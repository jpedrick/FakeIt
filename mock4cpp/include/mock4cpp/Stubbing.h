#ifndef Clouses_h__
#define Clouses_h__

#ifdef _WIN32
#define THROWS throw(...)
#else
#define THROWS noexcept(false)
#endif

#include <functional>
#include <type_traits>
#include "mockutils/traits.h"
#include "mockutils/DefaultValue.hpp"
#include "mock4cpp/Exceptions.h"

namespace mock4cpp {

struct MethodVerificationProgress {

	MethodVerificationProgress() {
	}

	~MethodVerificationProgress() THROWS {
	}

	void Never() {
		Times(0);
	}

	virtual void Once() {
		Times(1);
	}

	virtual void Twice() {
		Times(2);
	}

	virtual void AtLeastOnce() {
		VerifyInvocations(-1);
	}

	virtual void Times(const int times) {
		if (times < 0) {
			clearProgress();
			throw IllegalArgumentException(std::string("bad argument times:").append(std::to_string(times)));
		}
		VerifyInvocations(times);
	}

	virtual void startVerification() = 0;
protected:
	virtual void VerifyInvocations(const int times) = 0;
	virtual void clearProgress() = 0;

private:
	MethodVerificationProgress & operator=(const MethodVerificationProgress & other) = delete;
};

template<typename R, typename ... arglist>
struct NextFunctionStubbingProgress {

	virtual ~NextFunctionStubbingProgress() THROWS {
	}

	NextFunctionStubbingProgress<R, arglist...>&
	ThenReturn(const R& r) {
		return ThenDo([&r](...)->R {return r;});
	}

	NextFunctionStubbingProgress<R, arglist...>&
	ThenReturn() {
		return Do([](...)->R {DefaultValue::value<R>();});
	}

	template<typename E>
	NextFunctionStubbingProgress<R, arglist...>& ThenThrow(const E& e) {
		return ThenDo([e](...)->R {throw e;});
	}

	NextFunctionStubbingProgress<R, arglist...>& ThenDo(R (*method)(arglist...)) {
		return ThenDo(std::function < R(arglist...) > (method));
	}

	virtual auto ThenDo(std::function<R(arglist...)> method) -> NextFunctionStubbingProgress<R, arglist...>& = 0;

private:
	NextFunctionStubbingProgress & operator=(const NextFunctionStubbingProgress & other) = delete;
};

template<typename R, typename ... arglist>
struct FirstFunctionStubbingProgress: public virtual mock4cpp::MethodVerificationProgress {

	virtual ~FirstFunctionStubbingProgress() THROWS {
	}

	NextFunctionStubbingProgress<R, arglist...>&
	Return(const R& r) {
		return Do([&r](...)->R {return r;});
	}

	NextFunctionStubbingProgress<R, arglist...>&
	Return() {
		return Do([](...)->R {DefaultValue::value<R>();});
	}

	template<typename E>
	NextFunctionStubbingProgress<R, arglist...>& Throw(const E& e) {
		return Do([e](...)->R {throw e;});
	}

	virtual void operator=(std::function<R(arglist...)> method) {
		Do(method);
	}

	virtual auto Do(std::function<R(arglist...)> method) -> NextFunctionStubbingProgress<R, arglist...>& = 0;

	virtual void VerifyInvocations(const int times) override = 0;

private:
	FirstFunctionStubbingProgress & operator=(const FirstFunctionStubbingProgress & other) = delete;
};

template<typename R, typename ... arglist>
struct NextProcedureStubbingProgress {
	virtual ~NextProcedureStubbingProgress() THROWS {
	}

	NextProcedureStubbingProgress<R, arglist...>& ThenReturn() {
		return ThenDo([](...)->R {return DefaultValue::value<R>();});
	}

	template<typename E>
	NextProcedureStubbingProgress<R, arglist...>& ThenThrow(const E e) {
		return ThenDo([e](...)->R {throw e;});
	}

	virtual NextProcedureStubbingProgress<R, arglist...>& ThenDo(std::function<R(arglist...)> method) = 0;
private:
	NextProcedureStubbingProgress & operator=(const NextProcedureStubbingProgress & other) = delete;
};

template<typename R, typename ... arglist>
struct FirstProcedureStubbingProgress: public virtual mock4cpp::MethodVerificationProgress {

	virtual ~FirstProcedureStubbingProgress() THROWS {
	}

	NextProcedureStubbingProgress<R, arglist...>& Return() {
		return Do([](...)->R {return DefaultValue::value<R>();});
	}

	template<typename E>
	NextProcedureStubbingProgress<R, arglist...>& Throw(const E e) {
		return Do([e](...)->R {throw e;});
	}

	virtual void operator=(std::function<R(arglist...)> method) {
		Do(method);
	}

	virtual void VerifyInvocations(const int times) override = 0;

	virtual NextProcedureStubbingProgress<R, arglist...>& Do(std::function<R(arglist...)> method) = 0;
private:
	FirstProcedureStubbingProgress & operator=(const FirstProcedureStubbingProgress & other) = delete;
};


struct MethodStubbingInternal {

	~MethodStubbingInternal() = default;
	MethodStubbingInternal() = default;

	virtual void clearProgress() = 0;

	virtual void startStubbing() = 0;

	virtual void startVerification() = 0;

};

class VerifyFunctor {
public:
	VerifyFunctor() {
	}
	MethodVerificationProgress& operator()(const MethodVerificationProgress& verificationProgress) {
		MethodVerificationProgress& verificationProgressWithoutConst = (MethodVerificationProgress&)verificationProgress;
		verificationProgressWithoutConst.startVerification();
		return verificationProgressWithoutConst;
	}
}static Verify;

class WhenFunctor {
public:
	WhenFunctor() {
	}

	template<typename R, typename ... arglist>
	FirstProcedureStubbingProgress<R, arglist...>& operator()(const FirstProcedureStubbingProgress<R, arglist...>& stubbingProgress) {
		return (FirstProcedureStubbingProgress<R, arglist...>&) stubbingProgress;
	}

	template<typename R, typename ... arglist>
	FirstFunctionStubbingProgress<R, arglist...>& operator()(const FirstFunctionStubbingProgress<R, arglist...>& stubbingProgress) {
		return (FirstFunctionStubbingProgress<R, arglist...>&) stubbingProgress;
	}

}static When;

class StubFunctor {
private:
	void operator()() {
	}
public:
	StubFunctor() {
	}

	template<typename H>
	void operator()(const H& head) {
		H& headWithoutConst = (H&) head;
		auto& internal = dynamic_cast<MethodStubbingInternal&> (headWithoutConst);
		internal.startStubbing();
	}

	template<typename H, typename ... M>
	void operator()(const H& head, const M&... tail) {
		H& headWithoutConst = (H&) head;
		auto& internal = dynamic_cast<MethodStubbingInternal&> (headWithoutConst);
		internal.startStubbing();
		this->operator()(tail...);
	}

}static Stub;

}

#endif // Clouses_h__
