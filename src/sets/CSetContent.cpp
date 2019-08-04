#include "CSetContent.h"

CSetContent::CSetContent()
{
  // TODO Auto-generated constructor stub

}

CSetContent::~CSetContent()
{
  // TODO Auto-generated destructor stub
}

/*
      "oneOf": [
        {"$ref": "#/definitions/edgeElementSelector"},
        {"$ref": "#/definitions/nodeElementSelector"},
        {"$ref": "#/definitions/dbFieldValueSelector"},
        {"$ref": "#/definitions/setOperation"},
        {"$ref": "#/definitions/setReference"},
        {
          "description": "Either all nodes or all edges",
          "type": "object",
          "required": ["elementType"],
          "properties": {
            "elementType": {
              "type": "string",
              "enum": [
                "node",
                "edge"
              ]
            }
          },
          "additionalProperties": false
        }
      ]

 */
