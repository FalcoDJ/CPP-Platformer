#pragma once

#ifndef MATH_HPP
#define MATH_HPP

#include <random>

struct Math
{
    template <typename T>
    static T lerp(T from, T to, float easing)
    {
        return T(from + easing * (to - from));
    }

    template < typename T >
    static void shuffle( std::list<T>& lst ) // shuffle contents of a list
    {
        std::vector< std::reference_wrapper< const T > > vec( lst.begin(), lst.end() );
        std::shuffle( vec.begin(), vec.end(), std::mt19937{ std::random_device{}() } );
        std::list<T> shuffled_list {  vec.begin(), vec.end() };
        lst.swap(shuffled_list);
    }

    template<typename T>
    static T SIGN(T number)
    {
        return number < 0 ? -1 : (number != 0 ? 1 : 0);
    }

    template<typename T>
    static float move_toward(T p_from, T p_to, T p_delta) { return abs(p_to - p_from) <= p_delta ? p_to : p_from + Math::SIGN(p_to - p_from) * p_delta; }
};

#endif