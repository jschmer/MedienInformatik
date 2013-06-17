#include "StdAfx.h"
#include "Objekt.h"


/*----------------------------------------------------------------------------*/
/* get_normal   gibt den Normalenvektor der Oberflaeche eines Objekts an dem  */
/*   in 'v' bestimmten Punkt zurueck.                                         */
/* Aufrufparameter:    Punkt-Vektor, fuer den der N.-Vektor berechnet werd. s.*/
/*                     Datenstruktur in der der Objektpointer enthalten ist   */
/* Rueckgabeparameter: Berechneter Normalenvektor                             */
/*----------------------------------------------------------------------------*/

Vector Objekt::get_normal(Vector &v)
{
    return surface->get_normal(v);
} /* get_normal() */

