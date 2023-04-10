#ifndef ROBO_CHESS_ROBOCHESSDEFINES_H_
#define ROBO_CHESS_ROBOCHESSDEFINES_H_

//System headers

//Other libraries headers

//Own components headers

//Forward declarations

namespace motion {

enum MotionId {
  CHESS_MOVE_MOTION_ID,
  PARK_MOTION_ID,
  TURN_END_MOTION_ID
};

constexpr auto CHESS_MOVE_MOTION_SEQUENCE_NAME = "ChessMoveMotionSequence";
constexpr auto PARK_MOTION_SEQUENCE_NAME = "ParkMotionSequence";
constexpr auto TURN_END_MOTION_SEQUENCE_NAME = "TurnEndMotionSequence";

namespace chess_move {

constexpr auto GRASP_NAME = "ChessMoveGrasp";
constexpr auto TRANSPORT_NAME = "ChessMoveTransport";
constexpr auto PLACE_NAME = "ChessMovePlace";
constexpr auto RETRACT_NAME = "ChessMoveRetract";

} //namespace chess_move

namespace turn_end {

constexpr auto TRANSPORT_AND_CLICK_NAME = "TurnEndTransportAndClick";
constexpr auto RETRACT_NAME = "TurnEndRetract";

} //namespace turn_end

namespace park {

constexpr auto PARK_NAME = "Park";

} //namespace park

} //namespace motion

namespace gripper {

constexpr auto ACTIVATE_NAME = "GripperActivate";
constexpr auto OPEN_NAME = "GripperOpen";
constexpr auto CLOSE_NAME = "GripperClose";

} //namespace gripper

#endif /* ROBO_CHESS_ROBOCHESSDEFINES_H_ */