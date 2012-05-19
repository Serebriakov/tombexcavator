#include "bsw/logger/predicate.hpp"
#include "bsw/logger/record.hpp"
namespace bsw
{
  namespace logger
  {
    predicate_c::predicate_c ()
    {
    }

    predicate_c::~predicate_c ()
    {
    }

    not_predicate_c::not_predicate_c (const predicate_c* operand)
      : m_operand (operand)
    {
    }

    not_predicate_c::~not_predicate_c ()
    {
      if (m_operand)
	{
	  delete m_operand;
	}
    }

    bool not_predicate_c::call (const record_c* record) const
    {
      if (!m_operand)
	{
	  return true;
	}
      return !m_operand->call (record);
    }

    or_predicate_c::or_predicate_c (const predicate_c* operand1,
				    const predicate_c* operand2)
      : m_operand1 (operand1),
        m_operand2 (operand2)
    {
    }

    or_predicate_c::~or_predicate_c ()
    {
      if (m_operand1)
	{
	  delete m_operand1;
	}
      if (m_operand2)
	{
	  delete m_operand2;
	}
    }

    bool or_predicate_c::call (const record_c* record) const
    {
      bool r1 = m_operand1 ? m_operand1->call (record) : true;
      if (r1)
	{
	  return true;
	}
      return(m_operand2 ? m_operand2->call (record) : true);
    }

    and_predicate_c::and_predicate_c (const predicate_c* operand1,
				      const predicate_c* operand2)
      : m_operand1 (operand1),
        m_operand2 (operand2)
    {
    }

    and_predicate_c::~and_predicate_c ()
    {
      if (m_operand1)
	{
	  delete m_operand1;
	}
      if (m_operand2)
	{
	  delete m_operand2;
	}
    }

    bool and_predicate_c::call (const record_c* record) const
    {
      bool r1 = m_operand1 ? m_operand1->call (record) : true;
      if (!r1)
	{
	  return false;
	}
      return(m_operand2 ? m_operand2->call (record) : true);
    }

    if_predicate_c::if_predicate_c (const predicate_c* condition,
				    const predicate_c* yesBranch,
				    const predicate_c* noBranch) 
      : m_condition (condition),
        m_yesBranch (yesBranch),
        m_noBranch (noBranch)
    {
    }

    if_predicate_c::~if_predicate_c ()
    {
      if (m_condition)
	{
	  delete m_condition;
	}
      if (m_yesBranch)
	{
	  delete m_yesBranch;
	}
      if (m_noBranch)
	{
	  delete m_noBranch;
	}
    }

    bool if_predicate_c::call (const record_c* record) const
    {
      if (!m_condition)
	{
	  return true;
	}
      const predicate_c* branch;
      if (m_condition->call (record))
	{
	  branch = m_yesBranch;
	}
      else
	{
	  branch = m_noBranch;
	}
      if (branch)
	{
	  return branch->call (record);
	}
      else
	{
	  return true;
	}
    }

    level_eq_c::level_eq_c (level_t level) 
      : m_levelMin   (level),
        m_levelMax   (level),
        m_checkRange (false)
    {
    }

    level_eq_c::level_eq_c (level_t levelMin, level_t levelMax)
      : m_levelMin   (levelMin),
        m_levelMax   (levelMax),
        m_checkRange (true)
    {
    }

    bool level_eq_c::call (const record_c* record) const
    {
      level_t level = record->level ();
      if (m_checkRange)
	{
	  return(m_levelMin <= level && level <= m_levelMax);
	}
      return (m_levelMin == level);
    }


    line_num_eq_c::line_num_eq_c (unsigned int line) 
      : m_lineMin    (line),
        m_lineMax    (0),
        m_checkRange (false)
    {

    }

    line_num_eq_c::line_num_eq_c (unsigned int lineMin, unsigned int lineMax)
      : m_lineMin    (lineMin),
        m_lineMax    (lineMax),
        m_checkRange (true)
    {
    }

    bool line_num_eq_c::call (const record_c* record) const
    {
      unsigned int line = record->locus ().line ();
      if (m_checkRange)
	{
	  return(m_lineMin <= line && line <= m_lineMax);
	}
      return(line == m_lineMin);
    }


    file_name_eq_c::file_name_eq_c (const std::string& fileName) 
      : m_fileName (fileName)
    {
    }

    bool file_name_eq_c::call (const record_c* record) const
    {
      return(record->locus ().source () == m_fileName);
    }


    module_name_predicate_c::module_name_predicate_c (const char* moduleName)
      : m_moduleName (moduleName)
    {
    }

    bool module_name_predicate_c::call (const record_c* record) const
    {
      return (m_moduleName == record->locus ().module ());
    }
  } // ns logger
} // ns bsw

