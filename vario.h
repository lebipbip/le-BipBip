/*
 * vario.h
 *
 *  Created on: 17 sept. 2011
 *      Author: timothee
 */

#ifndef VARIO_H_
#define VARIO_H_
#include <stdbool.h>
#include <stdint.h>
void VarioToggleStationaryMode(void);
bool Vario(int32_t PressureDerivated);
void VarioDisableFor(int32_t time_ms);


#endif /* VARIO_H_ */
