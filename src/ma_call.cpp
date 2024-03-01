#include "mama.h"

namespace mavka::mama {
  MaCell ma_call_handler(MaMa* M,
                         MaCell cell,
                         MaArgs* args,
                         MaLocation location) {
  repeat:
    if (IS_OBJECT(cell)) {
      const auto object = cell.v.object;

      if (object->properties.contains(MAG_CALL)) {
        cell = object->properties[MAG_CALL];
        goto repeat;
      } else if (object->call) {
        FRAME_PUSH(
            new MaFrame(nullptr, object, M->frame_stack.top()->module, location));
        const auto result = object->call(M, object, args, location);
        FRAME_POP();
        return result;
      }
    }
    DO_THROW_CANNOT_CALL_CELL(cell);
  }

  MaCell ma_call(MaMa* M,
                 MaCell cell,
                 const std::vector<MaCell>& args,
                 MaLocation location) {
    return ma_call_handler(
        M, cell, new MaArgs(MA_ARGS_TYPE_POSITIONED, {}, args), location);
  }

  MaCell ma_call_named(MaMa* M,
                       MaCell cell,
                       const std::unordered_map<std::string, MaCell>& args,
                       MaLocation location) {
    return ma_call_handler(M, cell, new MaArgs(MA_ARGS_TYPE_NAMED, args, {}),
                           location);
  }
} // namespace mavka::mama