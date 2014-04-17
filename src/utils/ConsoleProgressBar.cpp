/* 
 * File:   ConsoleProgressBar.cpp
 * Copyright (C) 2013-2014  K M Masum Habib <masum.habib@gmail.com>
 * 
 * Created on April 8, 2013, 11:35 AM
 */

#include "ConsoleProgressBar.h"    

namespace utils{
    

ConsoleProgressBar::ConsoleProgressBar(string prefix, unsigned long expectedCount, 
    unsigned long count, bool livePercent):
    mExpectedCount(expectedCount),
    mCount(count < expectedCount ? count:expectedCount),
    mPrefix(prefix)
{
    mSuffix = "%";
    mSpace = ' ';
    mDot   = '.';
    mEnds  = '|';
}

void ConsoleProgressBar::step(unsigned long count){
    
    mCount += count;
    int nDots = (mCount*mnTotalDots)/mExpectedCount;
    
    while(nDots--){
        vout << vnormal << mDot;
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

void ConsoleProgressBar::complete(){
    vout << vnormal << mEnds;
}

void ConsoleProgressBar::start(){
    vout << vnormal << mPrefix << mEnds;
}

}

