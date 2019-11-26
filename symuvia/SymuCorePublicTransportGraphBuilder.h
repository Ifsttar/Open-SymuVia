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

#ifndef SYMUCOREPUBLICTRANSPORTGRAPHBUILDER_H
#define SYMUCOREPUBLICTRANSPORTGRAPHBUILDER_H

#include "tools.h"

#include <map>
#include <vector>

class Reseau;
class Arret;
class Trip;
class TripNode;
class Tuyau;
class TypeVehicule;
class SymuViaTripNode;
class SymuCoreGraphBuilder;
class SymuCoreDrivingGraphBuilder;
class PublicTransportFleet;
class Parking;
class Connexion;

namespace SymuCore {
    class Graph;
    class MultiLayersGraph;
    class Link;
    class Node;
    class VehicleType;
    class Populations;
    class Pattern;
    class PublicTransportLine;
    class AbstractPenalty;
    class Population;
    class Cost;
}

namespace boost {
    namespace posix_time {
        class ptime;
        class time_duration;
    }
}

class SymuCorePublicTransportGraphBuilder
{
public:

    SymuCorePublicTransportGraphBuilder();

	//! Constructeur
    SymuCorePublicTransportGraphBuilder(Reseau* pReseau);

	//! Destructeur
    virtual ~SymuCorePublicTransportGraphBuilder();

	void SetParent(SymuCoreGraphBuilder * pParent);
    SymuCoreGraphBuilder * GetParent();

    virtual bool CreatePublicTransportLayer(SymuCore::MultiLayersGraph * pGraph, SymuCoreDrivingGraphBuilder * pDrivingGraphBuilder);

    SymuCore::Node * GetOriginNodeFromConnexion(Connexion * pCon) const;
    SymuCore::Node * GetDestinationNodeFromTripNode(TripNode * pTripNode) const;
    SymuCore::Node * GetOriginNodeFromNode(SymuCore::Node * pNode) const;
    SymuCore::Node * GetDestinationNodeFromNode(SymuCore::Node * pNode) const;
    SymuCore::Node * GetNodeFromStop(Arret * pStop) const;

private:

    void BuildPatternsAndPenalties(SymuCore::Graph * pPublicTransportLayer);

    void BuildLinksBetweenLines(SymuCore::Graph * pPublicTransportLayer);

    void BuildLinksWithZones(SymuCore::Graph * pPublicTransportLayer, SymuCoreDrivingGraphBuilder * pDrivingGraphBuilder);

    void BuildLinksWithCarParks(SymuCore::Graph * pPublicTransportLayer, SymuCoreDrivingGraphBuilder * pDrivingGraphBuilder);

    void BuildLinksWithOrigins(SymuCore::Graph * pPublicTransportLayer, SymuCoreDrivingGraphBuilder * pDrivingGraphBuilder);

    void BuildLinksWithDestinations(SymuCore::Graph * pPublicTransportLayer, SymuCoreDrivingGraphBuilder * pDrivingGraphBuilder);
    
    void BuildPatterns(SymuCore::Graph * pPublicTransportLayer);

    void BuildPenalties(SymuCore::Graph * pPublicTransportLayer);

private:
    struct CompareLink {
        bool operator()(const SymuCore::Link* l1, const SymuCore::Link* l2) const;
    };

    struct CompareLine {
        bool operator()(const SymuCore::PublicTransportLine* l1, const SymuCore::PublicTransportLine* l2) const;
    };

protected:

    SymuCoreGraphBuilder*                                                                       m_pParent;
    Reseau*                                                                                     m_pNetwork;
    //pour l'instant, il n'y a qu'une flotte de transport public dans symuvia
    PublicTransportFleet*                                                                       m_pPublicTransportFleet;
    std::map<SymuCore::Link*, std::map<SymuCore::PublicTransportLine*, std::vector<Tuyau*>, CompareLine>, CompareLink>  m_mapLinksToTuyaux;
    std::map<Arret*, SymuCore::Node*, LessConnexionPtr<Arret> >                                 m_mapArretsToNode;
    std::map<SymuCore::Node*, Arret*>                                                           m_mapNodeToArrets;
    std::map<SymuCore::PublicTransportLine*, Trip*>                                             m_mapLineToTrip;
    std::vector<SymuCore::Link*>                                                                m_lstTerminationWalkingLinks;
    std::map<SymuCore::Link*, double>                                                           m_lstIntermediateWalkingLinks;
    std::map<SymuCore::Link*, double>                                                           m_lstInitialWalkingLinks;
    std::map<SymuCore::Link*, double>                                                           m_lstCarParkWalkingLinks;
    std::map<SymuCore::Node*, Parking*>                                                         m_lstCarParks;

    // Map entre les connexions (origines potentielles pour le SG) et les objets Node correspondants (on fait ceci car
    // le Dijkstra ne fonctionne pas si le dernier noeud de l'itinéraire (il va de la dest vers l'origine) n'a pas accès au service
    // Public Transport, du coup il faut créer des noeuds spéciaux...
    std::map<Connexion*, SymuCore::Node*>                                                      m_mapOriginConnexionsToNodes;
    std::map<SymuCore::Node*, SymuCore::Node*>                                                 m_mapPonctualOriginToSymuCoreNodes;
    // Idem pour les destinations (cas où on fait du 1 origin n destination)
    std::map<TripNode*, SymuCore::Node*>                                                       m_mapPonctualDestinationToNodes;
    std::map<SymuCore::Node*, SymuCore::Node*>                                                 m_mapPonctualDestinationToSymuCoreNodes;
};

#endif // SYMUCOREPUBLICTRANSPORTGRAPHBUILDER_H


