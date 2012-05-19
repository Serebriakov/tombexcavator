#ifndef __BSW_ERR_THROW_HPP__
#define __BSW_ERR_THROW_HPP__

#if !defined(_MSC_VER)
#define THROW_SPEC(X) throw (X)
#define NO_THROW_SPEC throw ()
#else
#define THROW_SPEC(X)
#define NO_THROW_SPEC throw ()
#endif


#endif
