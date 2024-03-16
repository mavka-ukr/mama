#ifndef MA_FRAME_H
#define MA_FRAME_H

struct MaFrame {
  MaObject* scope = nullptr;
  MaObject* object = nullptr;
  MaObject* module = nullptr;
  MaLocation location;
};

#endif // MA_FRAME_H