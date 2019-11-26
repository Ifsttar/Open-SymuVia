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

#pragma once
#ifndef SymuViaFleetH
#define SymuViaFleetH

#include "AbstractFleet.h"

#include <deque>

class SymuViaTripNode;

class SymuViaFleet : public AbstractFleet
{
public:
    SymuViaFleet();
    SymuViaFleet(Reseau * pNetwork);
    virtual ~SymuViaFleet();

    // Instanciation de l'objet spécifique à la flotte contenant les paramètres d'un véhicule liés à celle-ci
    virtual AbstractFleetParameters * CreateFleetParameters();

    // Initialisation de la flotte pour la simulation de trafic
    virtual void InitSimuTrafic(std::deque< TimeVariation<TraceDocTrafic> > & docTrafics);

    // Sorties spécifiques à la flotte dans les fichiers résultats
    virtual void SortieTrafic(DocTrafic *pXMLDocTrafic);

    // Traitements spécifiques à la flotte appelés lors du FinCalculTrafic
    virtual void FinCalculTrafic(Vehicule * pVeh);

    // Activation d'un véhicule sur ordre
    virtual void ActivateVehicle(double dbInstant, VehicleToCreate * pVehicleToCreate);

    // Gestion de la terminaison d'une étape par un véhicule de la flotte.
    virtual void OnCurrentLegFinished(boost::shared_ptr<Vehicule> pVehicle, VoieMicro * pDestinationEnterLane, double dbInstDestinationReached, double dbInstant, double dbTimeStep);

    // Met à jour le Trip en fonction des tuyaux parcourus
    virtual void SetLinkUsed(double dbInstant, Vehicule * pVeh, Tuyau * pLink);

    // Effectue les opérations à faire à l'activation d'un véhicule
    virtual void OnVehicleActivated(boost::shared_ptr<Vehicule> pVeh, double dbInstant);

    // Renvoie une map d'attributs à sortir de façon spécifique pour un véhicule une fois la simulation terminée
    virtual std::map<std::string, std::string> GetOutputAttributesAtEnd(Vehicule * pV);

protected:
    // Activation des véhicules pour le Trip passé en paramètres (si besoin)
    virtual std::vector<boost::shared_ptr<Vehicule> > ActivateVehiclesForTrip(double dbInstant, double dbTimeStep, Trip * pTrip);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sérialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // SymuViaFleetH