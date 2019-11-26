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
#ifndef NewellContextH
#define NewellContextH

#include "CarFollowingContext.h"

class NewellContext : public CarFollowingContext
{
public:
    NewellContext();
    NewellContext(Reseau * pNetwork, Vehicule * pVehicle, double dbInstant, bool bIsPostProcessing);
    virtual ~NewellContext();

    virtual void Build(double dbRange, CarFollowingContext * pPreviousContext);

    virtual void SetContext(const std::vector<boost::shared_ptr<Vehicule> > & leaders,
                            const std::vector<double> & leaderDistances,
                            const std::vector<VoieMicro*> & lstLanes,
                            double dbStartPosition,
                            CarFollowingContext * pOriginalContext = NULL,
                            bool bRebuild = false);

    double GetDeltaN() const;
    void SetDeltaN(double dbDeltaN);

    bool IsContraction() const;
    void SetContraction(bool bContraction);

    bool IsJorge() const;
    void SetJorge(bool bJorge);

    double GetJorgeStartTime() const;
    void SetJorgeEndTime(double dbEndTime);

    double GetJorgeEndTime() const;
    void SetJorgeStartTime(double dbEndTime);

    virtual void CopyTo(CarFollowingContext * pDestinationContext);

private:
    void ComputeDeltaN(NewellContext * pPreviousContext);

private:
    // Valeur du deltaN
    double m_dbDeltaN;

    // Indique si le véhicule est en train de se contracter (comportement agressif)
    bool m_bContraction;

    // indique si le véhicule utilise l'extenion Jorge
    bool m_bJorge;
    double m_dbJorgeStartTime;
    double m_dbJorgeEndTime;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sérialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif // NewellContextH
