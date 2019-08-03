#ifndef SRC_INTERVENTION_CVALUE_H_
#define SRC_INTERVENTION_CVALUE_H_

#include <iostream>

#include "traits/CTraitData.h"
#include "diseaseModel/CModel.h"

struct json_t;

class CValue
{
public:
  enum struct Type
  {
    boolean,
    number,
    healthState,
    traitValue
  };

  CValue(const Type & type = Type::number);

  CValue(const bool & boolean);

  CValue(const double & number);

  CValue(const CModel::state_t & healthState);

  CValue(const CTraitData::value & traitValue);

  CValue(const CValue & src);

  CValue(const json_t * json);

  CValue(std::istream & is);

  virtual ~CValue();

  const bool & toBoolean() const;

  const double & toNumber() const;

  const CModel::state_t & toHealthState() const;

  const CTraitData::value & toTraitValue() const;

  const Type & getType() const;

  const bool & isValid() const;

  void toBinary(std::ostream & os) const;

  bool operator<(const CValue & rhs) const;

  bool operator==(const CValue & rhs) const;

private:
  void createValue();

  void assignValue(const void *);

  void destroyValue();

  Type mType;
  void * mpValue;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CVALUE_H_ */
