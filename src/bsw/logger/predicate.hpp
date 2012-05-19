#ifndef __BSW_LOGGER_PREDICATE_HPP__
#define __BSW_LOGGER_PREDICATE_HPP__

#include <string>
#include "bsw/logger/level.hpp"
#include "bsw/ptr/ptr.hpp"
#include "bsw/bsw_api.hpp"
#include "bsw/mt/mutex.hpp"

namespace bsw 
{
  namespace logger
  {
    class record_c;

    class BSW_API predicate_c
    {
    public:
      predicate_c ();
      virtual ~predicate_c ();

      virtual bool call (const record_c* record) const = 0;
    };
    
    typedef utils::shared_ptr_c <predicate_c, mt::mutex_c> predicate_ptr_t;

    class BSW_API not_predicate_c : public predicate_c
    {
    public:
      not_predicate_c (const predicate_c* operand);
      ~not_predicate_c ();
      virtual bool call (const record_c* record) const;
    private:
      const predicate_c* m_operand;
    };

    class BSW_API or_predicate_c : public predicate_c
    {
    public:
      or_predicate_c (const predicate_c* operand1, 
		      const predicate_c* operand2);
      virtual ~or_predicate_c ();
      virtual bool call (const record_c* record) const;
    private:
      const predicate_c* m_operand1;
      const predicate_c* m_operand2;
    };

    class BSW_API and_predicate_c : public predicate_c
    {
    public:
      and_predicate_c (const predicate_c* operand1, 
		       const predicate_c* operand2);
      virtual ~and_predicate_c ();
      virtual bool call (const record_c* record) const;
    private:
      const predicate_c* m_operand1;
      const predicate_c* m_operand2;
    };

    class BSW_API if_predicate_c : public predicate_c
    {
    public:
      if_predicate_c (const predicate_c* condition,
		      const predicate_c* yesBranch,
		      const predicate_c* noBranch);
      virtual ~if_predicate_c ();
      virtual bool call (const record_c* record) const;
    private:
      const predicate_c* m_condition;
      const predicate_c* m_yesBranch;
      const predicate_c* m_noBranch;
    };

    class BSW_API level_eq_c : public predicate_c
    {
    public:
      level_eq_c (level_t level);
      level_eq_c (level_t levelMin, level_t levelMax);

      virtual bool call (const record_c* record) const;
    private:
      const level_t m_levelMin;
      const level_t m_levelMax;
      bool          m_checkRange;
    };

    class BSW_API line_num_eq_c : public predicate_c
    {
    public:
      line_num_eq_c (unsigned int line);
      line_num_eq_c (unsigned int lineMin, unsigned int lineMax);
      virtual bool call (const record_c* record) const;
    private:
      const unsigned int m_lineMin;
      const unsigned int m_lineMax;
      bool               m_checkRange;
    };

    class BSW_API file_name_eq_c : public predicate_c
    {
    public:
      file_name_eq_c (const std::string& fileName);
      virtual bool call (const record_c* record) const;
    private:
#if defined(_MSC_VER)
#pragma warning ( push )
#pragma warning ( disable: 4251 )
#endif
      const std::string m_fileName;
#if defined(_MSC_VER)
#pragma warning ( pop )
#endif    
    };

    class BSW_API module_name_predicate_c : public predicate_c
    {
    public:
      module_name_predicate_c (const char* moduleName);
      virtual bool call (const record_c* record) const;
    private:
#if defined(_MSC_VER)
#pragma warning ( push )
#pragma warning ( disable: 4251 )
#endif
      const std::string m_moduleName;
#if defined(_MSC_VER)
#pragma warning ( pop )
#endif    
    };
  } // ns logger
} // ns bsw



#endif
