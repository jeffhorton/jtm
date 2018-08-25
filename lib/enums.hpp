/*
 * Created by Dmitry Lyssenko, last modified July 22, 2018
 *
 * Macro definitions for ENUM stringification for in-class enum declarations:
 *
 * ENUM - will declare enum type w/o any possibility for stringification (mimics enum declaration)
 * ENUMSTR - will declare enum type with a possibility for later stringification
 * STRINGIFY - stringification of previously declared enum type with ENUMSTR macro
 *
 * trivial macro definitions making enum stringification easy:
 * declare enums now in a few simple steps:
 *  1. define a macro enlisting all enum items:
 *          #define MY_COLORS \
 *                  Red, \
 *                  Amber, \
 *                  Green
 *
 *  2. declare enums with ENUM macro if non-class declaration, or with ENUMSTR for in-class:
 *          ENUMSTR(trafficLightColors, MY_COLORS)
 *     - ENUM macro only declares trafficLightColors of enums, while ENUMSTR in addition
 *       declares an array of: static const char* trafficLightColors_[]
 *
 *  3. initialize an array of c-strings to generated by STRINGIFY macro (non in-class):
 *          STRINGIFY(SomeClass::trafficLightColors, MY_COLORS)
 *     - declares STRINGIFY macro outside of class, for non in-class declaration drop the
 *     cдass qualifier:
 *          STRINGIFY(trafficLightColors, MY_COLORS)
 *
 *  Now, enum trafficLightColors is defined, as well as its c-string representation:
 *          cout << "All traffic-light colors:";
 *          for(int i=0; i<COUNT_ARGS(MY_COLORS); ++i)
 *           cout << ' ' << SomeClass::trafficLightColors_str[i]; 
 *           // or equally:  cout << ' ' << STREN(trafficLightColors, i);
 *          cout << endl;
 *
 * Obvious caveat: enums declared that way do not allow value re-definition
 * However, consider enum value redefinition a bad programming style.
 * If required, provide enum-remapping facility instead
 */

#pragma once
#include "macrolib.h"




#define __COMMA_SEPARATED__(x) x,
#define __STR_COMMA_SEPARATED__(x) #x,


#define ENUM(enum_class, enums...) \
  enum enum_class { MACRO_TO_ARGS(__COMMA_SEPARATED__, enums) };


#define ENUMSTR(enum_class, enums...) \
  enum enum_class { MACRO_TO_ARGS(__COMMA_SEPARATED__, enums) }; \
  static const char * enum_class ## _str[];


#define STRINGIFY(enum_class, enums...) \
  const char * enum_class ## _str[] { MACRO_TO_ARGS(__STR_COMMA_SEPARATED__, enums) };

#define ENUMS(enum_class, enum_idx) enum_class ## _str[enum_idx]



/*
 * A trivial wrapper around std::exception
 * - to be used with enum stringification in classes
 *
 * Synopsis:
 * // 1. define ENUMSTR within the class, enumerating exception reasons
 *
 * class myClass {
 *   ...
 *  define THROWREASON InvalidInput, IncorrectUsage, WrongType
 *  ENUMSTR(ThrowReason, THROWREASON)
 *   ...
 *  EXCEPTION(ThrowReason)
 * };
 * STRINGIFY(myClass::ThrowReason, THROWREASON)
 * #undef THROWREASON
 *
 *
 * // 2. use in throwing defined exception reasons
 * throw EXP(WrongType);
 *
 *
 * // 3. Possible output upon non-handled exception:
 * libc++abi.dylib: terminating with uncaught exception of type stdException: WrongType
 *
 *
 * // 4. define catching exception:
 *  try { ... }     // something producing ThrowReason exception
 *  catch(stdException & e) {   // or std::exception & e, but then to access code()
 *                              // downcasting is required
 *   std::cout << "exception string: " << e.what() << std::endl;
 *   std::cout << "exception code: " << e.code() << std::endl;
 *  }
 *
 */


// return std::exception from classes;
// upon throwing a copy of the object is made (throwing is by value). In our case
// a shallow copy will suffice despite the pointer: the class is meant to be used
//with ENUMSTR / STRINGIFY, which are static data.
class stdException: public std::exception {
 public:
    const char *        what(void) const noexcept { return msg_; }
    const char *        where(void) const noexcept { return func_; }
    int                 code(void) const noexcept { return ec_; }
    stdException &      operator()(int r, const char *msg, const char *fnc)
                         { ec_ = r; msg_ = msg; func_ = fnc; return *this; }
 private:
    const char *        msg_{nullptr};
    const char *        func_{nullptr};
    int                 ec_{-1};
} __Expeption__;


// for in-class declaration
#define EXCEPTIONS(THROW_ENUM) \
    stdException & __exp__(THROW_ENUM reason, const char *where) const \
        { return __Expeption__(reason, THROW_ENUM ## _str[reason], where); }


#define EXP(TROW_REASON) __exp__(TROW_REASON, __PRETTY_FUNCTION__)










