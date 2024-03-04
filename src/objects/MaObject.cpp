#include "../mama.h"

namespace mavka::mama {
  void MaObject::Init(MaMa* M) {
    const auto object_structure_object = MaStructure::Create(M, "обʼєкт");
    M->global_scope->SetSubject("обʼєкт",
                                MA_MAKE_OBJECT(object_structure_object));
    M->object_structure_object = object_structure_object;
    M->structure_structure_object->structure = object_structure_object;
  }

  MaObject* MaObject::Instance(MaMa* M,
                               unsigned char type,
                               MaObject* structure_object,
                               void* d) {
    const auto object = new MaObject();
    object->type = type;
    object->d.ptr = d;
    object->structure = structure_object;
    for (const auto& method : structure_object->d.structure->methods) {
      const auto bound_diia_object = method->d.diia->Bind(M, object);
      object->properties.insert_or_assign(method->d.diia->name,
                                          MA_MAKE_OBJECT(bound_diia_object));
    }
    return object;
  }

  MaObject* MaObject::Empty(MaMa* M) {
    return MaObject::Instance(M, MA_OBJECT, M->object_structure_object,
                              nullptr);
  }

  void MaObject::Retain() {
    if (this->ref_count == 0) {
      this->ref_count = 1;
    } else {
      ++this->ref_count;
    }
  };

  void MaObject::Release() {
    if (this->ref_count == 0) {
      return;
    }
    --this->ref_count;
    if (this->ref_count == 0) {
      // todo: handle full delete
      delete this;
    }
  };

  bool MaObject::HasProperty(const std::string& name) {
    return this->properties.contains(name);
  }

  void MaObject::SetProperty(const std::string& name, MaCell value) {
    this->properties.insert_or_assign(name, value);
  }

  MaCell MaObject::GetProperty(const std::string& name) {
    return this->properties[name];
  }

  MaCell MaObject::GetPropertyOrEmpty(const std::string& name) {
    if (this->properties.contains(name)) {
      return this->properties[name];
    }
    RETURN_EMPTY();
  }
} // namespace mavka::mama