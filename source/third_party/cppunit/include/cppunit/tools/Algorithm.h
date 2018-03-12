#ifndef CPPUNIT_TOOLS_ALGORITHM_H_INCLUDED
#define CPPUNIT_TOOLS_ALGORITHM_H_INCLUDED

#include <cppunit/Portability.h>

CPPUNIT_NS_BEGIN

template<typename SequenceType, typename ValueType>
void
removeFromSequence( SequenceType &sequence, 
                    const ValueType &valueToRemove )
{
//   for ( unsigned int index =0; index < sequence.size(); ++index )
//   {
//      if ( sequence[ index ] == valueToRemove )
//         sequence.erase( sequence.begin() + index );
//   }

    // 以上的实现有Bug，如果sequence中有连续的重复的元素会删除不干净
    for ( unsigned int index =0; index < sequence.size();)
    {
        if ( sequence[ index ] == valueToRemove )
            sequence.erase( sequence.begin() + index );
        else
            ++index;
    }
}

CPPUNIT_NS_END


#endif // CPPUNIT_TOOLS_ALGORITHM_H_INCLUDED
