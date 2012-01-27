/*
 * filter.h
 *
 *  Created on: 23 sept. 2011
 *      Author: timothee
 */

#ifndef FILTER_H_
#define FILTER_H_
int32_t LP_Filter(int32_t x);
int64_t Filter(int32_t x);
void FilterReset(void);

#endif /* FILTER_H_ */
