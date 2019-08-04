#ifndef SRC_INTERVENTION_CVARIABLE_H_
#define SRC_INTERVENTION_CVARIABLE_H_

#include "utilities/CAnnotation.h"

class CVariable: public CAnnotation
{
public:
  enum struct Type
  {
    global,
    local
  };

  CVariable() = delete;

  CVariable(const CVariable & src);

  CVariable(const json_t * json);

  virtual ~CVariable();

  void fromJSON(const json_t * json);

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  const std::string & getId() const;

  const bool & isValid() const;

  /*
    "=",
    "*=",
    "/=",
    "+=",
    "-="
   */

private:
  std::string mId;
  Type mType;
  double mValue;
  double mResetValue;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CVARIABLE_H_ */
