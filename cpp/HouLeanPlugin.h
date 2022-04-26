#pragma once

#include <SOP/SOP_Node.h>

class SOP_HouLeanCore : public SOP_Node {
public:
  SOP_HouLeanCore(OP_Network *net, const char *name, OP_Operator *op);
  virtual ~SOP_HouLeanCore();

  static PRM_Template myTemplateList[];
  static OP_Node *    myConstructor(OP_Network *, const char *, OP_Operator *);

protected:
  /// Method to cook geometry for the SOP
  virtual OP_ERROR cookMySop(OP_Context &context);

private:
};
