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
#ifndef PositionH
#define PositionH

#include <vector>
#include <string>

#include <xercesc/util/XercesVersion.hpp>

namespace XERCES_CPP_NAMESPACE {
    class DOMNode;
}

namespace boost {
    namespace serialization {
        class access;
    }
}

class Connexion;
class Tuyau;
class VoieMicro;
class Reseau;
class Logger;
struct Point;

#define UNDEF_LINK_POSITION UNDEF_POSITION

class Position
{
public:
    Position();
    Position(Connexion * pConnection);
    Position(Tuyau * pLink, double dbPosition);
    virtual ~Position();

    // Chargement de la position à partir d'un noeud XML
    bool Load(XERCES_CPP_NAMESPACE::DOMNode * pXMLNode, const std::string & strElement, Reseau * pNetwork, Logger & loadingLogger);

    Connexion * GetConnection() const;
    void SetConnection(Connexion * pConnexion);

    void SetZone(const std::vector<Tuyau*> & tuyaux, const std::string & zoneID, Reseau * pNetwork);
    const std::vector<Tuyau*> & GetZone() const;
    bool IsInZone(Tuyau * pLink) const;
    void ComputeBarycentre(Point & pt) const;

    void SetLink(Tuyau * pLink);
    Tuyau * GetLink() const;
    Tuyau * GetDownstreamLink() const;

    void SetPosition(double dbPosition);
    double GetPosition() const;
    bool HasPosition() const;

    void SetOutside(bool bOutside);
    bool IsOutside() const;

    int GetLaneNumber() const;
    void SetLaneNumber(int laneNumber);
    bool HasLaneNumber() const;

    bool IsReached(VoieMicro * pLane, double laneLength, double startPositionOnLane, double endPositionOnLane) const;

    double GetRemainingLinkLength() const;

    bool IsNull() const;
    bool IsConnection() const;
    bool IsLink() const;
    bool IsZone() const;

protected:
    Connexion              *m_pConnection;
    Tuyau                  *m_pLink;
    std::vector<Tuyau*>    *m_pLstLinks;
    double                  m_dbPosition;
    bool                    m_bOutside;
    int                     m_LaneNumber;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sérialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif // PositionH