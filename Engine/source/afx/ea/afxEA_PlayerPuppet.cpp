
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX for MIT Licensed Open Source version of Torque 3D from GarageGames
// Copyright (C) 2015 Faust Logic, Inc.
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
//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "T3D/player.h"

#include "afx/afxChoreographer.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/ce/afxPlayerPuppet.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_PlayerPuppet 

class afxEA_PlayerPuppet : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxPlayerPuppetData* mover_data;
  afxConstraint*    obj_cons;

  void              do_runtime_substitutions();

public:
  /*C*/             afxEA_PlayerPuppet();

  void      ea_set_datablock(SimDataBlock*) override;
  bool      ea_start() override;
  bool      ea_update(F32 dt) override;
  void      ea_finish(bool was_stopped) override;

  void      getUnconstrainedPosition(Point3F& pos) override;
  void      getUnconstrainedTransform(MatrixF& xfm) override;
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_PlayerPuppet::afxEA_PlayerPuppet()
{
  mover_data = 0;
  obj_cons = 0;
}

void afxEA_PlayerPuppet::ea_set_datablock(SimDataBlock* db)
{
  mover_data = dynamic_cast<afxPlayerPuppetData*>(db);
}

bool afxEA_PlayerPuppet::ea_start()
{
  if (!mover_data)
  {
    Con::errorf("afxEA_PlayerPuppet::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  do_runtime_substitutions();

  afxConstraintID obj_id = mCons_mgr->getConstraintId(mover_data->obj_def);
  obj_cons = mCons_mgr->getConstraint(obj_id);

  Player* player = dynamic_cast<Player*>((obj_cons) ? obj_cons->getSceneObject() : 0);
  if (player)
    player->ignore_updates = true;

  return true;
}

bool afxEA_PlayerPuppet::ea_update(F32 dt)
{
  SceneObject* obj = (obj_cons) ? obj_cons->getSceneObject() : 0;

  if (obj && mIn_scope)
  {
    obj->setTransform(mUpdated_xfm);
  }

  return true;
}

void afxEA_PlayerPuppet::ea_finish(bool was_stopped)
{
  Player* player = dynamic_cast<Player*>((obj_cons) ? obj_cons->getSceneObject() : 0);
  if (player)
  {
    player->resetContactTimer();
    player->ignore_updates = false;
  }
}

void afxEA_PlayerPuppet::getUnconstrainedPosition(Point3F& pos)
{
  SceneObject* obj = (obj_cons) ? obj_cons->getSceneObject() : 0;
  if (obj)
    pos = obj->getRenderPosition();
  else
    pos.zero();
}

void afxEA_PlayerPuppet::getUnconstrainedTransform(MatrixF& xfm)
{
  SceneObject* obj = (obj_cons) ? obj_cons->getSceneObject() : 0;
  if (obj)
    xfm = obj->getRenderTransform();
  else
    xfm.identity();
}

void afxEA_PlayerPuppet::do_runtime_substitutions()
{
  // only clone the datablock if there are substitutions
  if (mover_data->getSubstitutionCount() > 0)
  {
    // clone the datablock and perform substitutions
    afxPlayerPuppetData* orig_db = mover_data;
    mover_data = new afxPlayerPuppetData(*orig_db, true);
    orig_db->performSubstitutions(mover_data, mChoreographer, mGroup_index);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_PlayerPuppetDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_PlayerPuppetDesc desc;

public:
  bool  testEffectType(const SimDataBlock*) const override;
  bool  requiresStop(const afxEffectWrapperData*, const afxEffectTimingData&) const override;
  bool  runsOnServer(const afxEffectWrapperData*) const override;
  bool  runsOnClient(const afxEffectWrapperData*) const override;

  afxEffectWrapper* create() const override { return new afxEA_PlayerPuppet; }
};

afxEA_PlayerPuppetDesc afxEA_PlayerPuppetDesc::desc;

bool afxEA_PlayerPuppetDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxPlayerPuppetData) == typeid(*db));
}

bool afxEA_PlayerPuppetDesc::requiresStop(const afxEffectWrapperData* ew, const afxEffectTimingData& timing) const
{
  return (timing.lifetime < 0);
}

bool afxEA_PlayerPuppetDesc::runsOnServer(const afxEffectWrapperData* ew) const
{
  U8 networking = ((const afxPlayerPuppetData*)ew->effect_data)->networking;
  return ((networking & (SERVER_ONLY | SERVER_AND_CLIENT)) != 0);
}

bool afxEA_PlayerPuppetDesc::runsOnClient(const afxEffectWrapperData* ew) const
{
  U8 networking = ((const afxPlayerPuppetData*)ew->effect_data)->networking;
  return ((networking & (CLIENT_ONLY | SERVER_AND_CLIENT)) != 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//