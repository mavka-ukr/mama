#include "../mama.h"

namespace mavka::mama {
  void MaObject::Init(MaMa* M) {
    const auto object_structure_object = MaStructure::Create(M, "обʼєкт");
    M->global_scope->SetSubject("обʼєкт", object_structure_object);
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
                                          MaValue::Object(bound_diia_object));
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

  bool MaObject::HasProperty(MaMa* M, const std::string& name) {
    return this->properties.contains(name);
  }

  void MaObject::SetProperty(MaMa* M,
                             const std::string& name,
                             const MaValue& value) {
    this->properties.insert_or_assign(name, value);
  }

  void MaObject::SetProperty(MaMa* M,
                             const std::string& name,
                             MaObject* value) {
    this->properties.insert_or_assign(name, MaValue::Object(value));
  }

  MaValue MaObject::GetProperty(MaMa* M, const std::string& name) {
    if (this->get) {
      return this->get(M, this, name);
    }
    return this->GetPropertyDirect(M, name);
  }

  MaValue MaObject::GetPropertyDirect(MaMa* M, const std::string& name) {
    if (this->properties.contains(name)) {
      return this->properties[name];
    }
    return MaValue::Empty();
  }
} // namespace mavka::mama