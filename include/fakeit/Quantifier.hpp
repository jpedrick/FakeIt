/*
 * Copyright (c) 2014 Eran Pe'er.
 *
 * This program is made available under the terms of the MIT License.
 *
 * Created on Mar 10, 2014
 */

#ifndef QUANTIFIER_HPP_
#define QUANTIFIER_HPP_

#include <functional>
#include <type_traits>

namespace fakeit {

struct Quantity {
	Quantity(const int quantity) :
			quantity(quantity) {
	}

	const int quantity;
};

template<typename R>
struct Quantifier: public Quantity {
	Quantifier(const int quantity, const R& value) :
			Quantity(quantity), value(value) {
	}

	const R& value;

	Quantifier<R> & operator()(const R& value) {
		this->value = value;
		return *this;
	}
};

template<>
struct Quantifier<void> : public Quantity {
	Quantifier(const int quantity) :
			Quantity(quantity) {
	}
};

struct QuantifierFunctor: public Quantifier<void> {
	QuantifierFunctor(const int quantity) :
			Quantifier<void>(quantity) {
	}

	template<typename R>
	Quantifier<R> operator()(const R& value) {
		return Quantifier<R>(quantity, value);
	}
} static Once(1);

template<int q>
struct Times :public Quantity{

	Times<q>():Quantity(q){}

	template<typename R>
	static Quantifier<R> of(const R& value) {
		return Quantifier<R>(q, value);
	}

	static Quantifier<void> Void() {
		return Quantifier<void>(q);
	}
};

#if defined (__GNUG__)
// Only supported by GCC

inline QuantifierFunctor operator"" _Times(unsigned long long n)
{
	return QuantifierFunctor((int)n);
}

inline QuantifierFunctor operator"" _Time(unsigned long long n)
{
	if (n != 1)
		throw std::invalid_argument("Only 1_Time is supported. Use X_Times (with s) if X is bigger than 1");
	return QuantifierFunctor((int)n);
}

#endif

}

#endif /* QUANTIFIER_HPP_ */
