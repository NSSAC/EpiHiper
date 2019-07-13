/*
 * MetaData.h
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#ifndef SRC_UTILITIES_METADATA_H_
#define SRC_UTILITIES_METADATA_H_

#include <string>

class json_t;

class Metadata
{
public:
  enum struct Type
  {
    Object,
    Array,
    Boolean,
    String,
    Integer,
    Double,
    Null,
    NotFound
  };

  Metadata();
  Metadata(const std::string & key, const std::string & value);
  Metadata(const std::string & key, const bool & value);
  Metadata(const std::string & key, const double & value);
  Metadata(const std::string & key, const int & value);
  Metadata(const std::string & key, const Metadata & value);
  Metadata(const Metadata & src);

  virtual ~Metadata();

  void set(const std::string & key, const std::string & value);
  void set(const std::string & key, const bool & value);
  void set(const std::string & key, const double & value);
  void set(const std::string & key, const int & value);
  void set(const std::string & key, const Metadata & value);

  bool contains(const std::string & key) const;
  Type getType(const std::string & key) const;
  std::string getString(const std::string & key) const;
  bool getBool(const std::string & key) const;
  double getDouble(const std::string & key) const;
  int getInt(const std::string & key) const;
  Metadata getObject(const std::string & key) const;

  void toBinary(std::ostream & os) const;
  bool fromBinary(std::istream & is);

private:
  Metadata(json_t * pObject);
  void set(const std::string & key, json_t * pValue);
  bool get(const std::string & key, json_t *& pValue) const;

  json_t * mpJson;
};


#endif /* SRC_UTILITIES_METADATA_H_ */
