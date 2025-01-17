//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _GUI_VARIABLEINSPECTOR_H_
#define _GUI_VARIABLEINSPECTOR_H_

#ifndef _GUI_INSPECTOR_H_
#include "gui/editor/guiInspector.h"
#endif
#ifndef _GUI_INSPECTOR_VARIABLEGROUP_H_
#include "gui/editor/inspector/variableGroup.h"
#endif


class GuiVariableInspector : public GuiInspector
{
   typedef GuiInspector Parent;

public:

   GuiVariableInspector();
   virtual ~GuiVariableInspector();

   DECLARE_CONOBJECT( GuiVariableInspector );
   DECLARE_CATEGORY( "Gui Editor" );

   void inspectObject( SimObject *object ) override {}

   virtual void loadVars( String searchString );

   void update();

   void startGroup(const char* name);
   void endGroup();
   void setGroupExpanded(const char* groupName, bool isExpanded);
   void setGroupsExpanded(bool isExpanded);

   void addField(const char* name, const char* label, const char* typeName, const char* description, 
      const char* defaultValue, const char* dataValues, const char* callbackName, SimObject* ownerObj);
   void addCallbackField(const char* name, const char* label, const char* typeName, const char* description,
      const char* defaultValue, const char* dataValues, const char* callbackName, SimObject* ownerObj);
   void setFieldEnabled(const char* name, bool enabled);
   void clearFields();

   void setAutoUpdate(bool doAutoUpdate) { mAutoUpdate = doAutoUpdate; }

protected:
   
   Vector<VariableField*> mFields;

   String mCurrentGroup;

   bool mAutoUpdate;

};

#endif // _GUI_VARIABLEINSPECTOR_H_
