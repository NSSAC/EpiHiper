#ifndef SRC_INTERVENTION_CVALUELIST_H_
#define SRC_INTERVENTION_CVALUELIST_H_

#include <set>

#include "intervention/CValue.h"

class CValueList : protected std::set< CValue >
{
public:
  typedef CValue::Type Type;
  typedef std::set< CValue >::const_iterator const_iterator;

  CValueList(const Type & type = Type::number);

  CValueList(const CValueList & src);

  CValueList(const json_t * json);

  CValueList(std::istream & is);

  bool append(const CValue & value);

  virtual ~CValueList();

  const_iterator begin() const;

  const_iterator end() const;

  size_t size() const;

  const bool & isValid() const;

  void toBinary(std::ostream & os) const;

private:
  Type mType;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CVALUELIST_H_ */
