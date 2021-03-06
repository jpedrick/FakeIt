/*
 * Copyright (c) 2014 Eran Pe'er.
 *
 * This program is made available under the terms of the MIT License.
 *
 * Created on Mar 10, 2014
 */

#ifndef VerifyNoOtherInvocationsFunctor_hpp_
#define VerifyNoOtherInvocationsFunctor_hpp_

#include <set>
#include <memory>

#include "fakeit/StubbingImpl.hpp"
#include "fakeit/Stubbing.hpp"
#include "fakeit/Sequence.hpp"
#include "fakeit/SortInvocations.hpp"
#include "fakeit/UsingFunctor.hpp"
#include "fakeit/VerifyNoOtherInvocationsVerificationProgress.hpp"

#include "mockutils/smart_ptr.hpp"
#include "mockutils/Macros.hpp"
#include "fakeit/FakeitContext.hpp"

namespace fakeit {
class VerifyNoOtherInvocationsFunctor {

	FakeitContext& _fakeit;

public:

	VerifyNoOtherInvocationsFunctor(FakeitContext& fakeit):_fakeit(fakeit) {
	}

	void operator()() {
	}

	template<typename ... list>
	VerifyNoOtherInvocationsVerificationProgress operator()(const ActualInvocationsSource& head, const list&... tail) {
		std::set<const ActualInvocationsSource*> invocationSources;
		invocationSources.insert(&head);
		collectInvocationSources(invocationSources, tail...);
		VerifyNoOtherInvocationsVerificationProgress progress{_fakeit,invocationSources};
		return progress;
	}
};

}

#endif // VerifyNoOtherInvocationsFunctor_hpp_
