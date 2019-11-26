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
#ifndef loi_emissionH
#define loi_emissionH

#include "SQLSymuvia.h"

namespace boost {
    namespace serialization {
        class access;
    }
}

class Reseau;

// La classe loi d'émission stocke les lois d'émission de tous les types de véhicules
// circulant sur le réseau
class Loi_emission{
private:
		DBSymuvia		    m_DBLoiEmission;            // Pointeur sur la base de données Loi Emission

        SQLSymuvia			m_DBRevetements;            // Liste des revêtements
        SQLSymuvia			m_DBAllures;                // Liste des allures
        SQLSymuvia			m_DBTypeVehicules;          // Liste des types de véhicules
        SQLSymuvia			m_DBVitesses;               // Liste des vitesses
        SQLSymuvia			m_DBVersion;                // Version de la base de données

        SQLSymuvia			m_DBPuissances;             // Liste des puissances*/

        double				m_dbFactPuis;

        bool                m_bLoaded;                  // permet de savoir si la connection à la BDD a été chargée (pour ne pas le refaire
                                                        // à la désérialisation si la connection est déjà effectuée)

        Reseau              *m_pNetwork;


public:
        static const int Nb_Octaves;          // nombre d'octaves pris en compte dans la simulation
        static const std::string nom_octave[8]; // tableau contenant les nom des octaves (sert en fait pour l'enregistrement des calcul acoustic au format MatFile)

        Loi_emission();         // constructeur
        Loi_emission(Reseau * pNetwork); // constructeur
        ~Loi_emission();        // destructeur

        void Init();

        std::string     GetVersion();

        bool    LoadDatabase();     // Charge les données de la base

        void    CalculEmissionBruit(double*, double, double, std::string, std::string);        // fonction principale
                // calcul l'emissionde bruti en fonction de paramètres passés et des données de la base de données
                // vitesse,, accélération, nom du revetement, type du vehicule et nom du vehicule


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sérialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif


