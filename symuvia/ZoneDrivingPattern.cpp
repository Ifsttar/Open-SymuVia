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
#include "ZoneDrivingPattern.h"

#include "ZoneDeTerminaison.h"
#include "Connection.h"
#include "reseau.h"

#include <Demand/SubPopulation.h>
#include <Demand/Population.h>


using namespace SymuCore;

ZoneDrivingPattern::ZoneDrivingPattern()
: SymuViaDrivingPattern(NULL, NULL)
{
}

ZoneDrivingPattern::ZoneDrivingPattern(SymuCore::Link* pLink, ZoneDeTerminaison * pZone, Connexion * pJunction, bool bIsOrigin)
: SymuViaDrivingPattern(pLink, pJunction->GetReseau())
{
    m_pZone = pZone;
    m_pJunction = pJunction;
    m_bIsOrigin = bIsOrigin;
}

ZoneDrivingPattern::~ZoneDrivingPattern()
{

}

ZoneDeTerminaison * ZoneDrivingPattern::getZone()
{
    return m_pZone;
}

std::string ZoneDrivingPattern::toString() const
{
    return "Area Pattern";
}

void ZoneDrivingPattern::fillMeasuredCostsForTravelTimesUpdatePeriod(int iTravelTimesUpdatePeriodIndex, const std::vector<SubPopulation*>& listSubPopulations, const std::map<SymuCore::SubPopulation*, SymuCore::CostFunction> & mapCostFunctions)
{
    // on regroupe les sous-population par macro-type :
    std::map< MacroType*, std::vector<SubPopulation*> > mapSubPopsByMacroType;
    for (size_t iSubPop = 0; iSubPop < listSubPopulations.size(); iSubPop++)
    {
        SubPopulation * pSubPop = listSubPopulations[iSubPop];
        mapSubPopsByMacroType[pSubPop->GetPopulation()->GetMacroType()].push_back(pSubPop);
    }

    // Pour chaque Macro-type
    for (std::map< MacroType*, std::vector<SubPopulation*> >::const_iterator iter = mapSubPopsByMacroType.begin(); iter != mapSubPopsByMacroType.end(); ++iter)
    {
        bool bMustComputeMarginals;
        if (m_pJunction->GetReseau()->GetComputeAllCosts())
        {
            bMustComputeMarginals = true;
        }
        else
        {
            bMustComputeMarginals = false;
            for (size_t iSubPop = 0; iSubPop < iter->second.size() && !bMustComputeMarginals; iSubPop++)
            {
                if (mapCostFunctions.at(iter->second[iSubPop]) == CF_Marginals)
                {
                    bMustComputeMarginals = true;
                }
            }
        }

        MacroType * pMacroType = iter->first;

        const TimeFrame<std::map<SubPopulation *, Cost>> & costVariation = getTemporalCosts().front().getTimeFrame((size_t)iTravelTimesUpdatePeriodIndex);

        // rmq. en mode sptial, dbMarginal prend la valeur du nombre de vÃ©hicules, Ã  stocker en plus du temps de parcours pour calculer le cout (marginal) a posteriori
        double dbMarginal, dbNbOfVehiclesForAllMacroTypes, dbTravelTimeForAllMacroTypes;
        double dbTravelTime = m_pZone->m_AreaHelper.GetTravelTime(m_bIsOrigin, m_pJunction, pMacroType, m_pZone, dbMarginal, dbNbOfVehiclesForAllMacroTypes, dbTravelTimeForAllMacroTypes);
        double dbMarginalCost;

        if (bMustComputeMarginals)
        {
            if (m_pJunction->GetReseau()->UseTravelTimesAsMarginalsInAreas())
            {
                dbMarginalCost = dbTravelTime;
            }
            else
            {
                if (m_pJunction->GetReseau()->UseSpatialTTMethod())
                {
                    dbMarginalCost = dbTravelTime;
                    m_marginalsHelper.SetNbVehicles(iTravelTimesUpdatePeriodIndex, costVariation.getStartTime(), costVariation.getEndTime(), pMacroType,
                        dbMarginal, dbNbOfVehiclesForAllMacroTypes, dbTravelTimeForAllMacroTypes);
                }
                else
                {
                    dbMarginalCost = dbMarginal;
                }
            }
        }

        for (size_t iSubPop = 0; iSubPop < iter->second.size(); iSubPop++)
        {
            SubPopulation * pSubPop = iter->second.at(iSubPop);

            Cost & cost = costVariation.getData()->operator[](pSubPop);
            cost.setTravelTime(dbTravelTime);

            if (mapCostFunctions.at(pSubPop) == CF_TravelTime)
            {
                cost.setUsedCostValue(dbTravelTime);
                if (bMustComputeMarginals)
                {
                    cost.setOtherCostValue(CF_Marginals, dbMarginalCost);
                }
            }
            else if (mapCostFunctions.at(pSubPop) == CF_Marginals)
            {
                cost.setUsedCostValue(dbMarginalCost);
                cost.setOtherCostValue(CF_TravelTime, dbTravelTime);
            }
            else
            {
                assert(false);
            }
            
        }
    }
}

void ZoneDrivingPattern::postProcessCosts(const std::vector<SubPopulation*>& listSubPopulations, const std::map<SymuCore::SubPopulation*, SymuCore::CostFunction> & mapCostFunctions)
{
    if (m_pJunction->GetReseau()->UseSpatialTTMethod() && !m_pJunction->GetReseau()->UseTravelTimesAsMarginalsInAreas())
    {
        // on regroupe les sous-population par macro-type :
        std::map< MacroType*, std::vector<std::pair<CostFunction, SubPopulation*> > > mapSubPopsByMacroType;
        for (size_t iSubPop = 0; iSubPop < listSubPopulations.size(); iSubPop++)
        {
            SubPopulation * pSubPop = listSubPopulations[iSubPop];
            CostFunction subPopCostFunc = mapCostFunctions.at(pSubPop);
            if (subPopCostFunc == SymuCore::CF_Marginals || m_pJunction->GetReseau()->GetComputeAllCosts())
            {
                mapSubPopsByMacroType[pSubPop->GetPopulation()->GetMacroType()].push_back(std::pair<CostFunction, SubPopulation*>(subPopCostFunc, pSubPop));
            }
        }

        std::map<MacroType*, bool> forbiddenMacroTypes;
        for (std::map<MacroType*, std::vector<std::pair<CostFunction, SubPopulation*> > >::const_iterator iter = mapSubPopsByMacroType.begin(); iter != mapSubPopsByMacroType.end(); ++iter)
        {
            MacroType * pMacroType = iter->first;

            bool bIsForbidden = !m_pZone->m_AreaHelper.HasPath(m_pJunction, pMacroType, m_bIsOrigin);
            forbiddenMacroTypes[pMacroType] = bIsForbidden;
        }

        m_marginalsHelper.ComputeMarginal(mapSubPopsByMacroType, forbiddenMacroTypes, getTemporalCosts().front(), m_pJunction->GetReseau()->GetMaxMarginalsValue(), 1.0);
    }
}

void ZoneDrivingPattern::fillCost(const std::map<TypeVehicule*, SymuCore::SubPopulation *> &listSubPopulations)
{
    const TimeFrame<std::map<SubPopulation *, Cost> > & costVariation = getTemporalCosts()[0].getTimeFrame(0);

    for (std::map<TypeVehicule*, SymuCore::SubPopulation *>::const_iterator iterTV = listSubPopulations.begin(); iterTV != listSubPopulations.end(); ++iterTV)
    {
        Cost & cost = costVariation.getData()->operator[](iterTV->second);
        cost.setTravelTime(0);
        cost.setUsedCostValue(0);
    }
}


