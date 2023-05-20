#ifndef SRC_MATH_CCValueInstance_H_
#  define SRC_MATH_CCValueInstance_H_

#  include <memory>

#include "math/CEdgeProperty.h"
#include "math/CNodeProperty.h"
#include "math/CValueList.h"
#include "math/CComputableSet.h"

class CObservable;
class CVariable;
class CSizeOf;

struct json_t;

class CValueInstance
{
public:
  enum struct ValueType
  {
    Value,
    ValueList,
    Observable,
    NodeProperty,
    EdgeProperty,
    Variable,
    Sizeof,
    __SIZE
  };

  static bool compatible(const CValueInstance & lhs, const CValueInstance & rhs);

  CValueInstance();

  CValueInstance(const CValueInstance & src);

  ~CValueInstance();

  void fromJSON(const json_t * json, bool writable);

  CValueInterface value(const CNode * pNode) const;

  CValueInterface value(const CEdge * pEdge) const;

  CValueInterface & value() const;

  CValueList & valueList() const;

  CValueInterface::Type interfaceType() const;

  bool inherit() const;

  const bool & isValid() const;

  const ValueType & getType() const;

  CComputable * getPrerequisite() const;

private:
  ValueType mType;
  bool mWritable;
  bool mValid;
  CComputable * mpPrerequisites;

  std::shared_ptr< CValue > mpValue;
  std::shared_ptr< CValueList > mpValueList;
  CObservable * mpObservable;
  std::shared_ptr< CNodeProperty > mpNodeProperty;
  std::shared_ptr< CEdgeProperty > mpEdgeProperty;
  CVariable * mpVariable;
  std::shared_ptr< CSizeOf > mpSizeOf;
};

#endif // SRC_MATH_CCValueInstance_H_
