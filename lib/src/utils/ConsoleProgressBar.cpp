/* 
 * File:   ConsoleProgressBar.cpp
 * Copyright (C) 2013-2014  K M Masum Habib <masum.habib@gmail.com>
 * 
 * Created on April 8, 2013, 11:35 AM
 */

#include "utils/ConsoleProgressBar.h"    

namespace utils{
    

ConsoleProgressBar::ConsoleProgressBar(string prefix, unsigned long expectedCount, 
    unsigned long count, bool scaled, bool livePercent):
    mExpectedCount(expectedCount),
    mCount(count < expectedCount ? count:expectedCount),
    mscaled(scaled),
    mPrefix(prefix)
{
    mSuffix = "%";
    mSpace = ' ';
    mDot   = '.';
    mEnds  = '|';
}

void ConsoleProgressBar::step(unsigned long count){
    
    mCount += count;
    
    int nDots = mscaled ? (mCount*mnTotalDots)/mExpectedCount : mCount;
    
    while(nDots--){
        if (mscaled){
            vout << vnormal << mDot;
        }else{
            int myId = vout.myId();
            vout.myId(0);            
            vout << vnormal << mDot;
            vout.myId(myId);
        }
        mCount = 0;
    }    
}

ConsoleProgressBar& ConsoleProgressBar::operator+= (unsigned long newCount){

    step(newCount);
    return *this;
}

ConsoleProgressBar operator+ (ConsoleProgressBar lhs, 
        unsigned long newCount){
    lhs += newCount;
    return lhs;
}

ConsoleProgressBar& ConsoleProgressBar::operator++(){
    *this += 1;
    return *this;
}

ConsoleProgressBar ConsoleProgressBar::operator++(int){
    ConsoleProgressBar tmp(*this);
    operator++();
    return tmp;
}

void ConsoleProgressBar::expectedCount(unsigned long expectedCount){
    mExpectedCount = expectedCount;
}

void ConsoleProgressBar::complete(){
    vout << vnormal << mEnds;
}

void ConsoleProgressBar::start(){
    vout << vnormal << mPrefix << mEnds;
}


void ConsoleProgressBar::reset(){
    mCount = 0;
}


}

