/*
 * Annotation.h
 *
 *  Created on: Jun 22, 2019
 *      Author: shoops
 */

#ifndef SRC_UTILITIES_ANNOTATION_H_
#define SRC_UTILITIES_ANNOTATION_H_

#include <string>

struct json_t;

class Annotation
{
  public:
    Annotation();

    Annotation(const Annotation & src);

    virtual ~Annotation();

    void fromJSON(const json_t * json);

    const std::string & getAnnId() const;

    const std::string & getAnnLabel() const;

    const std::string & getAnnDescription() const;

  protected:
    std::string mAnnId;
    std::string mAnnLabel;
    std::string mAnnDescription;
};

#endif /* SRC_UTILITIES_ANNOTATION_H_ */
