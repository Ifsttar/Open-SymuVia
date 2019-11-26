/* ----------------------------------------------------------------------
* OPEN-SYMUVIA (http://open-symuvia.ifsttar.fr). This file is part of OPEN-SYMUVIA.
*
* OPEN-SYMUVIA is an open-source traffic simulator.
*
* Copyright (C) - IFSTTAR, ENTPE - Ludovic Leclercq, Cécile Bécarie
*
* Open-SymuVia is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
* 
* OPEN-SYMUVIA is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lessed General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA or 
* see <http://ww.gnu.org/licenses/>
*
* For more information, please consult: <http://open-symuvia.ifsttar.fr> or 
* send an email to cecile.becarie@ifsttar.fr
*
* ----------------------------------------------------------------------*/

#include "stdafx.h"
#include "MFDSensor.h"

#include "EdieSensor.h"
#include "PonctualSensor.h"
#include "../TraceDocTrafic.h"
#include "../SystemUtil.h"
#include "../tuyau.h"
#include "../voie.h"
#include "../BriqueDeConnexion.h"
#include "../ConnectionPonctuel.h"
#include "../vehicule.h"

#include <boost/serialization/vector.hpp>

MFDSensor::MFDSensor() : AbstractSensor()
{
    m_bIsEdie = false;

    m_bGeomLength1 = 0;
    m_bGeomLength2 = 0;
    m_bGeomLength3 = 0;

    m_bEnableStrictSensors = false;

    m_bSimpleMFD = false;
}

MFDSensor::MFDSensor(Reseau * pNetwork,
                     SensorsManager * pParentSensorsManager,
                     double dbPeriodeAgregation,
                     const char* strNom,
                     const std::vector<Tuyau*> & Tuyaux,
                     const std::vector<double> & dbPosDebut,
                     const std::vector<double> & dbPosFin,
                     const std::vector<int> & eTuyauType,
                     bool bIsMFD,
                     bool bIsSimpleMFD)
                     : AbstractSensor(strNom)
{
    m_bIsEdie = !bIsMFD;

    m_bSimpleMFD = bIsSimpleMFD;

    m_bGeomLength1 = 0;
    m_bGeomLength2 = 0;
    m_bGeomLength3 = 0;

    // on regarde si on doit utiliser des capteurs pour les débits stricts (uniquement si capteur MFD avec un seul tronçon défini par l'utilisateur)
    int nbNonInternalLink = 0;
    for(size_t i = 0; i < eTuyauType.size() && nbNonInternalLink <= 1; i++)
    {
        if(eTuyauType[i] == EdieSensor::TT_Link)
        {
            nbNonInternalLink++;
        }
    }
    m_bEnableStrictSensors = nbNonInternalLink == 1;

    for (size_t i = 0; i < Tuyaux.size(); i++)
    {
        Tuyau * pLink = Tuyaux[i];
        EdieSensor * pEdie = new EdieSensor(strNom, pLink, dbPosDebut[i], dbPosFin[i], eTuyauType[i], this);
        pLink->getLstRelatedSensorsBySensorsManagers()[pParentSensorsManager].push_back(pEdie);
        lstSensors.push_back(pEdie);

        if (bIsMFD && eTuyauType[i] == EdieSensor::TT_Link && !m_bSimpleMFD)
        {
            // *************************************
            // Définition des capteurs de débit
            // *************************************

            // Cas de la sortie du réseau (la connexion aval est ue sortie ou un parking)
            if (pLink->get_Type_aval() == 'S' || pLink->get_Type_aval() == 'P')
            {
                // On utilise dans ce cas forcément un capteur à la fin du tronçon
                PonctualSensor * pNewSensor = new PonctualSensor(pLink, pLink->GetLength());
                pLink->getLstRelatedSensorsBySensorsManagers()[pParentSensorsManager].push_back(pNewSensor);
                lstPonctualSensors.push_back(pNewSensor);
                lstNonStrictPonctualSensors.push_back(pNewSensor);
                lstStrictPonctualSensors.push_back(pNewSensor);
            }
            else
            {
                // Ajout d'un capteur pour chaque tronçon aval accessible à partir du tuyau amont et qui sort de la zone MFD
                Tuyau * pTuyAm = pLink;
                Connexion * pConnexionAval;
                if(pTuyAm->GetBriqueAval())
                {
                    pConnexionAval = pTuyAm->GetBriqueAval();
                }
                else
                {
                    pConnexionAval = pTuyAm->getConnectionAval();
                }
                for(size_t iTuyAv = 0; iTuyAv < pConnexionAval->m_LstTuyAv.size(); iTuyAv++)
                {
                    Tuyau * pTuyAv = pConnexionAval->m_LstTuyAv[iTuyAv];
                    // Le tronçon aval ne doit pas être dans la zone MFD
                    if(std::find(Tuyaux.begin(), Tuyaux.end(), pTuyAv) == Tuyaux.end())
                    {
                        // Le tuyau aval doit être accessible depuis le tuyau amont (sinon pas la peine de mettre un capteur couteux en temps de traitement)
                        if(pConnexionAval->IsMouvementAutorise(pTuyAm, pTuyAv, NULL, NULL))
                        {
                            PonctualSensor * pNewSensor = new PonctualSensor(pTuyAv, 0);
                            pTuyAv->getLstRelatedSensorsBySensorsManagers()[pParentSensorsManager].push_back(pNewSensor);
                            pNewSensor->SetTuyauAmont(pTuyAm);
                            lstPonctualSensors.push_back(pNewSensor);
                            lstNonStrictPonctualSensors.push_back(pNewSensor);
                        }
                    }
                }

                // Si besoin (cas d'un capteur MFD avec un seul tronçon, pour lequel on doit ressortir les données strictes)
                if(m_bEnableStrictSensors)
                {
                    PonctualSensor * pNewSensor = new PonctualSensor(pLink, pLink->GetLength());
                    pLink->getLstRelatedSensorsBySensorsManagers()[pParentSensorsManager].push_back(pNewSensor);
                    lstPonctualSensors.push_back(pNewSensor);
                    lstStrictPonctualSensors.push_back(pNewSensor);
                }
            }


            // *************************************
            // Calcul des longueurs géométriques
            // *************************************
            double dbLength = pLink->GetLength() * pLink->getNb_voies();
            m_bGeomLength1 += dbLength;
            m_bGeomLength2 += dbLength;
            m_bGeomLength3 += dbLength;

            // Pour m_bGeomLength1, ajout de la distance entre les extremités du tronçon et les barycentres des briques amont et aval
            if (pLink->GetBriqueAmont())
            {
                Point * pBaryCentre = pLink->GetBriqueAmont()->CalculBarycentre();
                Point * pPoint = pLink->GetExtAmont();
                m_bGeomLength1 += sqrt((pPoint->dbX-pBaryCentre->dbX)*(pPoint->dbX-pBaryCentre->dbX)
                    +(pPoint->dbY-pBaryCentre->dbY)*(pPoint->dbY-pBaryCentre->dbY)
                    +(pPoint->dbZ-pBaryCentre->dbZ)*(pPoint->dbZ-pBaryCentre->dbZ)) * pLink->getNb_voies();
                delete pBaryCentre;
            }
            if (pLink->GetBriqueAval())
            {
                BriqueDeConnexion * pBrique = pLink->GetBriqueAval();
                Point * pBaryCentre = pBrique->CalculBarycentre();
                Point * pPoint = pLink->GetExtAval();
                m_bGeomLength1 += sqrt((pPoint->dbX-pBaryCentre->dbX)*(pPoint->dbX-pBaryCentre->dbX)
                    +(pPoint->dbY-pBaryCentre->dbY)*(pPoint->dbY-pBaryCentre->dbY)
                    +(pPoint->dbZ - pBaryCentre->dbZ)*(pPoint->dbZ - pBaryCentre->dbZ)) * pLink->getNb_voies();
                delete pBaryCentre;

                // Pour m_bGeomLength2, ajout de la moyenne des longueurs des mouvements aval
                double dbCumulLength = 0;
                int nbMoves = 0;
                Tuyau * pTuyAm = pLink;
                for(int iLaneAm = 0; iLaneAm < pTuyAm->getNb_voies(); iLaneAm++)
                {
                    for(size_t iTuyAv = 0; iTuyAv < pBrique->m_LstTuyAv.size(); iTuyAv++)
                    {
                        Tuyau * pTuyAv = pBrique->m_LstTuyAv[iTuyAv];
                        for(int iLaneAv = 0; iLaneAv < pTuyAv->getNb_voies(); iLaneAv++)
                        {
                            std::vector<Tuyau*> tuyaux;
                            if(pBrique->GetTuyauxInternes(pTuyAm, iLaneAm, pTuyAv, iLaneAv, tuyaux))
                            {
                                for(size_t iTuyInt = 0; iTuyInt < tuyaux.size(); iTuyInt++)
                                {
                                    dbCumulLength += tuyaux[iTuyInt]->GetLength();
                                }
                                nbMoves++;
                            }
                        }
                    }
                }
                m_bGeomLength2 += (dbCumulLength / (double)nbMoves)  * pLink->getNb_voies();
            }
        }
    }

    for(size_t iPonctual = 0; iPonctual < lstPonctualSensors.size(); iPonctual++)
    {
        lstPonctualSensors[iPonctual]->InitData(pNetwork, dbPeriodeAgregation);
    }
}

MFDSensor::~MFDSensor()
{
    for(size_t iEdie = 0; iEdie < lstSensors.size(); iEdie++)
    {
        delete lstSensors[iEdie];
    }
    for(size_t iPonctual = 0; iPonctual < lstPonctualSensors.size(); iPonctual++)
    {
        delete lstPonctualSensors[iPonctual];
    }
}

std::string MFDSensor::GetSensorPeriodXMLNodeName() const
{
    if(m_bIsEdie)
    {
        return "CAPTEURS_EDIE";
    }
    else
    {
        return "CAPTEURS_MFD";
    }
}

const std::vector<EdieSensor*> &MFDSensor::GetLstSensors()
{
    return lstSensors;
}
    
bool MFDSensor::IsEdie()
{
    return m_bIsEdie;
}

void MFDSensor::CalculInfoCapteur(Reseau * pNetwork, double dbInstant, bool bNewPeriod, double dbInstNewPeriode, boost::shared_ptr<Vehicule> pVeh)
{
    // rien a calculer ici pour les capteurs internes constituant le capteur MFD car ils sont rattachés au SensorsManager parent
    // et ont été traités dans le CalculInfoCapteur de ce SensorsManager (ou indépendamment si pas de SensorsManager parent)
}

void MFDSensor::AddMesoVehicle(double dbInstant, Vehicule * pVeh, Tuyau * pLink, Tuyau * pDownstreamLink, double dbLengthInLink)
{
    for(size_t iEdie = 0; iEdie < lstSensors.size(); iEdie++)
    {
        lstSensors[iEdie]->AddMesoVehicle(dbInstant, pVeh, pLink, pDownstreamLink, dbLengthInLink);
    }
    for(size_t iPonctual = 0; iPonctual < lstPonctualSensors.size(); iPonctual++)
    {
        lstPonctualSensors[iPonctual]->AddMesoVehicle(dbInstant, pVeh, pLink, pDownstreamLink, dbLengthInLink);
    }
}

void MFDSensor::WriteDef(DocTrafic* pDocTrafic)
{
    if(IsEdie())
    {
        lstSensors.front()->WriteDef(pDocTrafic);
    }
    else
    {
        std::vector<Tuyau*> lstTuyaux;
        for(size_t iEdie = 0; iEdie < lstSensors.size(); iEdie++)
        {
            if(lstSensors[iEdie]->GetTuyauType() == EdieSensor::TT_Link)
            {
                lstTuyaux.push_back(lstSensors[iEdie]->GetTuyau());
            }
        }
        pDocTrafic->AddDefCapteurMFD( GetUnifiedID(), m_bEnableStrictSensors, lstTuyaux );
    }
}

void MFDSensor::Write(double dbInstant, Reseau * pReseau, double dbPeriodeAgregation, double dbDebut, double dbFin, const std::deque<TraceDocTrafic* > & docTrafics, CSVOutputWriter * pCSVOutput)
{
    if(!docTrafics.empty())
    {
        if(m_bIsEdie)
        {
            assert(lstSensors.size() == 1);
            lstSensors.front()->Write(dbInstant, pReseau, dbPeriodeAgregation, dbDebut, dbFin, docTrafics, pCSVOutput);
        }
        else
        {
            std::deque<TraceDocTrafic* >::const_iterator itDocTraf;
            double dbDistanceTotale = 0.0;
            double dbDistanceTotaleStricte = 0.0;
            double dbTempsTotalPasse = 0.0;
            double dbTempsTotalPasseStrict = 0.0;
            double dbDebitSortie = 0.0;
			double dbIntDebitSortie = 0.0;			// Débit de sortie (véh disparaissnt à l'intérieur dee la zone)
			double dbTransDebitSortie = 0.0;		// Débit de sortie (véh traversant la zone)
            double dbDebitSortieStrict = 0.0;
            double dbLongueurDeplacement = 0.0;
            double dbLongueurDeplacementStricte = 0.0;
            double dbVitesseSpatiale = 0.0;
            double dbVitesseSpatialeStricte = 0.0;
            double dbConcentrationEdie = 0.0;
			double dbDebitEdie = 0.0;

            for(size_t j=0; j< lstSensors.size(); j++)
            {
                EdieSensor * pCptEdie = lstSensors[j];    

                // calcul du cumul
                double dbTTD = pCptEdie->GetData().GetTotalTravelledDistance();
                double dbTTT = pCptEdie->GetData().GetTotalTravelledTime();
				int nNbRemVeh = pCptEdie->GetData().GetTotalNumberOfRemovalVehicles();

                dbTempsTotalPasse += dbTTT;
                dbDistanceTotale += dbTTD;

                if( pCptEdie->GetTuyauType() ==  EdieSensor::TT_Link) 
                {
                    dbTempsTotalPasseStrict += dbTTT;
                    dbDistanceTotaleStricte += dbTTD;
					//dbDebitSortie += (double)nNbRemVeh / (dbFin - dbDebut);
					dbDebitSortie += (double)nNbRemVeh;
					//dbIntDebitSortie += (double)nNbRemVeh / (dbFin - dbDebut);
					dbIntDebitSortie += (double)nNbRemVeh;
                }
            }

            int nbVehFranchissantTotal = 0;
            
            for(size_t j=0; j< lstNonStrictPonctualSensors.size(); j++)
            {
                int nNbVehFranchissant = 0;
                for(int k=0; k<lstNonStrictPonctualSensors[j]->GetTuyau()->getNb_voies(); k++)
                {
                    nNbVehFranchissant += lstNonStrictPonctualSensors[j]->GetData().data.find(nullptr)->second.nNbVehFranchissant[k];
                }
                nbVehFranchissantTotal += nNbVehFranchissant;
                if(dbFin > dbDebut)
                {
                    //dbDebitSortie += (double)nNbVehFranchissant / ((dbFin - dbDebut) * lstNonStrictPonctualSensors[j]->GetTuyau()->getNb_voies());
					//dbDebitSortie += (double)nNbVehFranchissant / ((dbFin - dbDebut) );
					dbDebitSortie += (double)nNbVehFranchissant;
					//dbTransDebitSortie += (double)nNbVehFranchissant / ((dbFin - dbDebut));
					dbTransDebitSortie += (double)nNbVehFranchissant;
                }
            }

            int nbVehFranchissantStrictTotal = 0;
            for(size_t j=0; j< lstStrictPonctualSensors.size(); j++)
            {
                int nNbVehFranchissant = 0;
                for(int k=0; k<lstStrictPonctualSensors[j]->GetTuyau()->getNb_voies(); k++)
                {
                    nNbVehFranchissant += lstStrictPonctualSensors[j]->GetData().data.find(nullptr)->second.nNbVehFranchissant[k];
                }
                nbVehFranchissantStrictTotal += nNbVehFranchissant;
                if(dbFin > dbDebut)
                {
                    //dbDebitSortieStrict += (double)nNbVehFranchissant / ((dbFin - dbDebut) * lstStrictPonctualSensors[j]->GetTuyau()->getNb_voies());
					dbDebitSortieStrict += (double)nNbVehFranchissant / ((dbFin - dbDebut) );
                }
            }

            if(dbTempsTotalPasse != 0)
            {
                dbVitesseSpatiale = dbDistanceTotale / dbTempsTotalPasse;
            }
            if(dbTempsTotalPasseStrict != 0)
            {
                dbVitesseSpatialeStricte = dbDistanceTotaleStricte / dbTempsTotalPasseStrict;
            }
            if(dbDebitSortie != 0 && dbFin > dbDebut)
            {
                dbLongueurDeplacement = dbDistanceTotale / nbVehFranchissantTotal;
            }
            if(dbDebitSortieStrict != 0 && dbFin > dbDebut)
            {
                dbLongueurDeplacementStricte = dbDistanceTotaleStricte / nbVehFranchissantStrictTotal;
            }

            // Pour éviter la division par 0 
			// Calcul du débit et de la concentration en utilisant les formules d'Edie et la longueur géométrique barycentre pour la longueur du capteur
            if(dbFin > dbDebut && m_bGeomLength2 > 0)
            {
				dbDebitEdie			= dbDistanceTotale  / ((dbFin - dbDebut) * m_bGeomLength2);
                dbConcentrationEdie = dbTempsTotalPasse / ((dbFin - dbDebut) * m_bGeomLength2);
            }

            // écriture
            for( itDocTraf = docTrafics.begin(); itDocTraf != docTrafics.end(); itDocTraf++ )
            {
                (*itDocTraf)->AddInfoCapteurMFD(m_strNom,                                        // identifiant capteur
                    m_bEnableStrictSensors,
                    SystemUtil::ToString(3, m_bGeomLength1),   // Longueur géométrique barycentre
                    SystemUtil::ToString(3, m_bGeomLength2),   // Longueur géométrique mvtaval
                    SystemUtil::ToString(3, m_bGeomLength3),   // Longueur géométrique stricte
                    SystemUtil::ToString(3, dbDistanceTotale), // Distance totale parcourue
                    SystemUtil::ToString(3, dbDistanceTotaleStricte),  // Distance totale parcourue stricte
                    SystemUtil::ToString(3, dbTempsTotalPasse),        // Temps total passé
                    SystemUtil::ToString(3, dbTempsTotalPasseStrict),  // Temps total passé stricte
                    SystemUtil::ToString(3, dbDebitSortie),            // Débit de sortie
					SystemUtil::ToString(3, dbIntDebitSortie),            // Débit de sortie du aux véhicules disparaissant dans la zone
					SystemUtil::ToString(3, dbTransDebitSortie),            // Débit de sortie du aux véhicules ayant traversé la zone
                    SystemUtil::ToString(3, dbDebitSortieStrict),      // Débit de sortie strict
                    SystemUtil::ToString(3, dbLongueurDeplacement),        // Longueur de déplacement
                    SystemUtil::ToString(3, dbLongueurDeplacementStricte), // Longueur de déplacement stricte
                    SystemUtil::ToString(3, dbVitesseSpatiale),            // Vitesse spatiale
                    SystemUtil::ToString(3, dbVitesseSpatialeStricte),     // Vitesse spatiale stricte
                    SystemUtil::ToString(3, dbConcentrationEdie),               // Concentration calculée à partir des formules d'Edie
					SystemUtil::ToString(3, dbDebitEdie)               // Débit calculée à partir des formules d'Edie
                    ); 
            }
        }
    }
}

void MFDSensor::PrepareNextPeriod()
{
    for(size_t iEdie = 0; iEdie < lstSensors.size(); iEdie++)
    {
        lstSensors[iEdie]->PrepareNextPeriod();
    }
    for(size_t iPonctual = 0; iPonctual < lstPonctualSensors.size(); iPonctual++)
    {
        lstPonctualSensors[iPonctual]->PrepareNextPeriod();
    }
}


AbstractSensorSnapshot * MFDSensor::TakeSnapshot()
{
    MFDSensorSnapshot * pResult = new MFDSensorSnapshot();
    for(size_t iEdie = 0; iEdie < lstSensors.size(); iEdie++)
    {
        EdieSensorData * pData = (EdieSensorData*)lstSensors[iEdie]->TakeSnapshot();
        pResult->edieData.push_back(*pData);
        delete pData;
    }
    for(size_t iPonctual = 0; iPonctual < lstPonctualSensors.size(); iPonctual++)
    {
        PonctualSensorSnapshot * pData = (PonctualSensorSnapshot*)lstPonctualSensors[iPonctual]->TakeSnapshot();
        pResult->ponctualData.push_back(*pData);
        delete pData;
    }
    return pResult;
}

void MFDSensor::Restore(Reseau * pNetwork, AbstractSensorSnapshot * backup)
{
    MFDSensorSnapshot * pSnapshot = (MFDSensorSnapshot*)backup;
    for(size_t iEdie = 0; iEdie < lstSensors.size(); iEdie++)
    {
        lstSensors[iEdie]->Restore(pNetwork, &pSnapshot->edieData[iEdie]);
    }
    for(size_t iPonctual = 0; iPonctual < lstPonctualSensors.size(); iPonctual++)
    {
        lstPonctualSensors[iPonctual]->Restore(pNetwork, &pSnapshot->ponctualData[iPonctual]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sérialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template void MFDSensorSnapshot::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void MFDSensorSnapshot::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void MFDSensorSnapshot::serialize(Archive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(AbstractSensorSnapshot);
    ar & BOOST_SERIALIZATION_NVP(edieData);
    ar & BOOST_SERIALIZATION_NVP(ponctualData);
}

template void MFDSensor::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void MFDSensor::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void MFDSensor::serialize(Archive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(AbstractSensor);
    ar & BOOST_SERIALIZATION_NVP(lstSensors);
    ar & BOOST_SERIALIZATION_NVP(lstPonctualSensors);
    ar & BOOST_SERIALIZATION_NVP(lstStrictPonctualSensors);
    ar & BOOST_SERIALIZATION_NVP(lstNonStrictPonctualSensors);
    ar & BOOST_SERIALIZATION_NVP(m_bEnableStrictSensors);
    ar & BOOST_SERIALIZATION_NVP(m_bIsEdie);
    ar & BOOST_SERIALIZATION_NVP(m_bGeomLength1);
    ar & BOOST_SERIALIZATION_NVP(m_bGeomLength2);
    ar & BOOST_SERIALIZATION_NVP(m_bGeomLength3);
    ar & BOOST_SERIALIZATION_NVP(m_bSimpleMFD);
}

