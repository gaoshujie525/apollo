/******************************************************************************
 * Copyright 2019 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "modules/data/tools/smart_recorder/emergency_mode_trigger.h"

#include "cyber/common/log.h"
#include "modules/control/proto/control_cmd.pb.h"
#include "modules/localization/proto/localization.pb.h"

namespace apollo {
namespace data {

using apollo::canbus::Chassis;

EmergencyModeTrigger::EmergencyModeTrigger() {
  trigger_name_ = "EmergencyModeTrigger";
}

bool EmergencyModeTrigger::Init(const SmartRecordTrigger& trigger_conf) {
  // Have to instantiate the wanted classes here that do nothing but
  // register themselves to global factory which then provides reflections later
  apollo::canbus::Chassis chassis_instance;
  apollo::control::ControlCommand control_instance;
  apollo::localization::LocalizationEstimate pose_instance;
  return TriggerBase::Init(trigger_conf);
}

void EmergencyModeTrigger::Pull(const RecordMessage& msg) {
  if (!trigger_obj_->enabled()) {
    return;
  }
  if (msg.channel_name == "/apollo/canbus/chassis") {
    apollo::canbus::Chassis chassis_msg;
    chassis_msg.ParseFromString(msg.content);
    if (cur_driving_mode_ == Chassis::COMPLETE_AUTO_DRIVE &&
        chassis_msg.driving_mode() == Chassis::EMERGENCY_MODE) {
      AINFO << "emergency mode trigger is pulled: " << msg.time << " - "
            << msg.channel_name;
      TriggerIt(msg.time);
    }
    cur_driving_mode_ = chassis_msg.driving_mode();
  }
}

}  // namespace data
}  // namespace apollo
