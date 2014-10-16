#pragma once


///////////////////////////////////////////////////////////////////////////////

/**
 * This observer realizes the pull-observation pattern.
 * Its up to the subject to decide in what circumstances
 * the observer gets updated, and thus speicfying the context
 * of observer will be operating in (what data should it pull from the subject).
 *
 * If you want to notify about different events, simply create different specializations
 * of this class and make the subject implement different specializations
 * of the subject class.
 */
template<typename SubjectType, typename MessageEnum>
class Observer
{
public:
   virtual ~Observer() {}

   virtual void update(SubjectType& subject) = 0;

   virtual void update(SubjectType& subject, const MessageEnum& msg) = 0;
};

///////////////////////////////////////////////////////////////////////////////
