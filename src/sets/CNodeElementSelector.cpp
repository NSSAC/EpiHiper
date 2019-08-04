#include "CNodeElementSelector.h"

CNodeElementSelector::CNodeElementSelector()
{
  // TODO Auto-generated constructor stub

}

CNodeElementSelector::~CNodeElementSelector()
{
  // TODO Auto-generated destructor stub
}

/*
      "oneOf": [
        {
          "required": [
            "elementType",
            "operator",
            "left",
            "right"
          ],
          "properties": {
            "elementType": {
              "type": "string",
              "enum": ["node"]
            },
            "operator": {"$ref": "#/definitions/comparisonOperator"},
            "left": {
              "type": "object",
              "required": ["node"],
              "properties": {
                "node": {"$ref": "#/definitions/nodeProperty"}
              }
            },
            "right": {"$ref": "#/definitions/value"}
          }
        },
        {
          "description": "",
          "required": [
            "elementType",
            "operator",
            "left",
            "right"
          ],
          "properties": {
            "elementType": {
              "type": "string",
              "enum": ["node"]
            },
            "operator": {
              "description": "",
              "type": "string",
              "enum": [
                "in",
                "not in"
              ]
            },
            "left": {
              "type": "object",
              "required": ["node"],
              "properties": {
                "node": {"$ref": "#/definitions/nodeProperty"}
              }
            },
            "right": {"$ref": "#/definitions/valueList"}
          }
        },
        {
          "description": "",
          "required": [
            "elementType",
            "operator",
            "left",
            "right"
          ],
          "properties": {
            "elementType": {
              "type": "string",
              "enum": ["node"]
            },
            "operator": {
              "description": "",
              "type": "string",
              "enum": ["withPropertyIn"]
            },
            "left": {
              "type": "object",
              "required": ["node"],
              "properties": {
                "node": {"$ref": "#/definitions/nodeProperty"}
              }
            },
            "right": {"$ref": "#/definitions/valueList"}
          }
        },
        {
          "description": "",
          "required": [
            "elementType",
            "operator",
            "selector"
          ],
          "properties": {
            "elementType": {
              "type": "string",
              "enum": ["node"]
            },
            "operator": {
              "description": "",
              "type": "string",
              "enum": ["withIncomingEdgeIn"]
            },
            "selector": {"$ref": "#/definitions/setContent"}
          }
        },
        {
          "description": "A filter selecting nodes from the external person trait database.",
          "oneOf": [
            {
              "description": "A table in the external person trait database.",
              "type": "object",
              "required": [
                "elementType",
                "table"
              ],
              "additionalProperties": false,
              "properties": {
                "elementType": {
                  "type": "string",
                  "enum": ["node"]
                },
                "table": {"$ref": "#/definitions/uniqueIdRef"}
              }
            },
            {
              "description": "A filter returining nodes if the result of comparing left and right values with the operator is true.",
              "required": [
                "elementType",
                "operator",
                "left",
                "right"
              ],
              "properties": {
                "elementType": {
                  "type": "string",
                  "enum": ["node"]
                },
                "operator": {"$ref": "#/definitions/comparisonOperator"},
                "left": {"$ref": "#/definitions/dbField"},
                "right": {
                  "type": "object",
                  "description": "",
                  "oneOf": [
                    {"$ref": "#/definitions/dbFieldValue"},
                    {"$ref": "#/definitions/observable"}
                  ]
                }
              }
            },
            {
              "description": "A filter returining nodes if the left field value is or is not in the right list.",
              "required": [
                "elementType",
                "operator",
                "left",
                "right"
              ],
              "properties": {
                "elementType": {
                  "type": "string",
                  "enum": ["node"]
                },
                "operator": {
                  "description": "",
                  "type": "string",
                  "enum": [
                    "in",
                    "not in"
                  ]
                },
                "left": {"$ref": "#/definitions/dbField"},
                "right": {
                  "type": "object",
                  "description": "",
                  "oneOf": [
                    {"$ref": "#/definitions/dbFieldValueList"},
                    {"$ref": "#/definitions/dbFieldValueSelector"}
                  ]
                }
              }
            }
          ]
        }
      ]

 */
