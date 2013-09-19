/* 
 * File:   filter.h
 * Author: tiggr
 *
 * Created on 8. April 2013, 22:53
 */

#ifndef FILTER_H
#define	FILTER_H

class Complementary {
        public:
            void setTau(float in);
            float filter(float speed, float dt, float angle);
            Complementary() { angleSum=0; tau=0.8; }
            
        protected:
            float angleSum;
            float tau;
};

#endif	/* FILTER_H */

