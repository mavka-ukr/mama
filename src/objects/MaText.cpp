#include <csetjmp>

#include "../external/utf8/utf8.h"
#include "../mama.h"

// TODO: переписати це все

namespace mavka::mama {

  static jmp_buf buf;

  std::size_t utf8_len(const std::string& str) {
    return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}
        .from_bytes(str)
        .size();
  }

  std::string utf8_substr(const std::string& str,
                          std::size_t start,
                          std::size_t length) {
    const auto utf32 =
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}
            .from_bytes(str);
    return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}
        .to_bytes(utf32.substr(start, length));
  }

  std::vector<std::string> utf8_chars(const std::string& str) {
    const auto utf32 =
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}
            .from_bytes(str);
    std::vector<std::string> chars;
    for (const auto& c : utf32) {
      chars.push_back(
          std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}
              .to_bytes(c));
    }
    return chars;
  }

  size_t utf8_find_index(const std::string& str, const std::string& substr) {
    const auto utf32 =
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}
            .from_bytes(str);
    const auto utf32_substr =
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}
            .from_bytes(substr);
    return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}
        .to_bytes(utf32.substr(0, utf32.find(utf32_substr)))
        .size();
  }

  size_t MaText::Length() const {
    return utf8_len(this->data);
  }

  std::string MaText::Substr(size_t start, size_t length) const {
    return utf8_substr(this->data, start, length);
  }

  // розбити
  MaCell MaText_SplitNativeDiiaFn(MaMa* M,
                                  MaObject* o,
                                  MaArgs* args,
                                  const MaLocation& location) {
    const auto cell = args->Get(0, "роздільник");
    if (cell.IsObject() && cell.IsObjectText()) {
      const auto delim = cell.v.object->d.string->data;
      if (delim.empty()) {
        const auto list_object = MaList::Create(M);
        for (const auto& c : utf8_chars(o->d.diia_native->me->d.string->data)) {
          list_object->d.list->data.push_back(
              MaCell::Object(MaText::Create(M, c)));
        }
        RETURN_OBJECT(list_object);
      }
      std::vector<std::string> result;
      std::string current;
      for (const auto& c : utf8_chars(o->d.diia_native->me->d.string->data)) {
        if (c == delim) {
          result.push_back(current);
          current.clear();
        } else {
          current += c;
        }
      }
      result.push_back(current);
      const auto list_object = MaList::Create(M);
      for (const auto& s : result) {
        list_object->d.list->data.push_back(
            MaCell::Object(MaText::Create(M, s)));
      }
      RETURN_OBJECT(list_object);
    } else {
      RETURN_ERROR(new MaError(MaCell::Object(MaText::Create(
          M, "Для дії \"розбити\" потрібен текстовий аргумент."))));
    }
  }

  // замінити
  MaCell MaText_ReplaceNativeDiiaFn(MaMa* M,
                                    MaObject* o,
                                    MaArgs* args,
                                    const MaLocation& location) {
    const auto oldVal = args->Get(0, "старе");
    if (!(oldVal.IsObject() && oldVal.IsObjectText())) {
      RETURN_ERROR(new MaError(MaCell::Object(MaText::Create(
          M, "Для дії \"замінити\" перший аргумент повинен бути текстом."))));
    }
    const auto newVal = args->Get(1, "нове");
    if (!(newVal.IsObject() && newVal.IsObjectText())) {
      RETURN_ERROR(new MaError(MaCell::Object(MaText::Create(
          M, "Для дії \"замінити\" другий аргумент повинен бути текстом."))));
    }
    const auto first_string = oldVal.AsText()->data;
    const auto second_string = newVal.AsText()->data;
    std::string new_string;
    for (std::size_t i = 0; i < o->d.diia_native->me->d.string->Length(); i++) {
      const auto substr = o->d.diia_native->me->d.string->Substr(i, 1);
      if (substr == first_string) {
        new_string += second_string;
      } else {
        new_string += substr;
      }
    }
    RETURN(MaCell::Object(MaText::Create(M, new_string)));
  }

  // починається
  MaCell MaText_StartsWithNativeDiiaFn(MaMa* M,
                                       MaObject* o,
                                       MaArgs* args,
                                       const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.IsObject() && cell.IsObjectText()) {
      if (o->d.diia_native->me->d.string->data.find(
              cell.v.object->d.string->data) == 0) {
        return MaCell::Yes();
      }
      return MaCell::No();
    } else {
      RETURN_ERROR(new MaError(MaCell::Object(MaText::Create(
          M, "Для дії \"починається\" потрібен текстовий аргумент."))));
    }
  }

  // закінчується
  MaCell MaText_EndsWithNativeDiiaFn(MaMa* M,
                                     MaObject* o,
                                     MaArgs* args,
                                     const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.IsObject() && cell.IsObjectText()) {
      const auto text = cell.AsText();
      if (o->d.diia_native->me->d.string->Length() < text->Length()) {
        return MaCell::No();
      }
      if (o->d.diia_native->me->d.string->Substr(
              o->d.diia_native->me->d.string->Length() - text->Length(),
              text->Length()) == text->data) {
        return MaCell::Yes();
      }
      return MaCell::No();
    } else {
      RETURN_ERROR(new MaError(MaCell::Object(MaText::Create(
          M, "Для дії \"закінчується\" потрібен текстовий аргумент."))));
    }
  }

  // обтяти
  MaCell MaText_TrimNativeDiiaFn(MaMa* M,
                                 MaObject* o,
                                 MaArgs* args,
                                 const MaLocation& location) {
    return MaCell::Object(MaText::Create(
        M, internal::tools::trim(o->d.diia_native->me->d.string->data)));
  }

  // чародія_додати
  MaCell MaText_MagAddNativeDiiaFn(MaMa* M,
                                   MaObject* o,
                                   MaArgs* args,
                                   const MaLocation& location) {
    const auto arg_cell = args->Get(0, "значення");
    if (arg_cell.IsEmpty()) {
      return MaCell::Object(
          MaText::Create(M, o->d.diia_native->me->d.string->data + "пусто"));
    }
    if (arg_cell.IsNumber()) {
      return MaCell::Object(
          MaText::Create(M, o->d.diia_native->me->d.string->data +
                                ma_number_to_string(arg_cell.v.number)));
    }
    if (arg_cell.IsYes()) {
      return MaCell::Object(
          MaText::Create(M, o->d.diia_native->me->d.string->data + "так"));
    }
    if (arg_cell.IsNo()) {
      return MaCell::Object(
          MaText::Create(M, o->d.diia_native->me->d.string->data + "ні"));
    }
    if (arg_cell.IsObject()) {
      if (arg_cell.IsObjectText()) {
        return MaCell::Object(
            MaText::Create(M, o->d.diia_native->me->d.string->data +
                                  arg_cell.v.object->d.string->data));
      }
    }
    RETURN_ERROR(new MaError(MaCell::Object(
        MaText::Create(M, "Неможливо додати до тексту обʼєкт типу \"" +
                              arg_cell.GetName() + "\"."))));
  }

  // чародія_містить
  MaCell MaText_MagContainsNativeDiiaFn(MaMa* M,
                                        MaObject* o,
                                        MaArgs* args,
                                        const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.IsObject() && cell.IsObjectText()) {
      if (o->d.diia_native->me->d.string->data.find(
              cell.v.object->d.string->data) != std::string::npos) {
        return MaCell::Yes();
      }
      return MaCell::No();
    } else {
      RETURN_ERROR(new MaError(MaCell::Object(MaText::Create(
          M, "Для дії \"чародія_містить\" потрібен текстовий аргумент."))));
    }
  }

  // чародія_отримати
  MaCell MaText_MagGetElementNativeDiiaFn(MaMa* M,
                                          MaObject* o,
                                          MaArgs* args,
                                          const MaLocation& location) {
    const auto cell = args->Get(0, "позиція");
    if (cell.IsNumber()) {
      const auto i = cell.AsNumber();
      if (i < o->d.diia_native->me->d.string->Length()) {
        const auto substr = o->d.diia_native->me->d.string->Substr(i, 1);
        return MaCell::Object(MaText::Create(M, substr));
      }
    }
    RETURN_EMPTY();
  }

  // чародія_перебір
  MaCell MaText_MagIteratorNativeDiiaFn(MaMa* M,
                                        MaObject* o,
                                        MaArgs* args,
                                        const MaLocation& location) {
    RETURN_ERROR(new MaError(
        MaCell::Object(MaText::Create(M, "Дія \"" + std::string(MAG_ITERATOR) +
                                             "\" тимчасово недоступна."))));
  }

  // чародія_число
  MaCell MaText_MagNumberNativeDiiaFn(MaMa* M,
                                      MaObject* o,
                                      MaArgs* args,
                                      const MaLocation& location) {
    RETURN_NUMBER(std::stod(o->d.diia_native->me->d.string->data));
  }

  MaCell MaString_GetHandler(MaMa* M, MaObject* me, const std::string& name) {
    if (name == "довжина") {
      return MaCell::Number(me->AsText()->Length());
    }
    return me->GetPropertyOrEmpty(name);
  }

  MaObject* MaText::Create(MaMa* M, const std::string& value) {
    const auto string = new MaText();
    string->data = value;
    const auto string_object = MaObject::Instance(
        M, MA_OBJECT_STRING, M->text_structure_object, string);
    string_object->get = MaString_GetHandler;
    string_object->SetProperty(
        "розбити", MaDiiaNative::Create(M, "розбити", MaText_SplitNativeDiiaFn,
                                        string_object));
    string_object->SetProperty(
        "замінити",
        MaDiiaNative::Create(M, "замінити", MaText_ReplaceNativeDiiaFn,
                             string_object));
    string_object->SetProperty(
        "починається",
        MaDiiaNative::Create(M, "починається", MaText_StartsWithNativeDiiaFn,
                             string_object));
    string_object->SetProperty(
        "закінчується",
        MaDiiaNative::Create(M, "закінчується", MaText_EndsWithNativeDiiaFn,
                             string_object));
    string_object->SetProperty(
        "обтяти", MaDiiaNative::Create(M, "обтяти", MaText_TrimNativeDiiaFn,
                                       string_object));
    string_object->SetProperty(
        MAG_ADD, MaDiiaNative::Create(M, MAG_ADD, MaText_MagAddNativeDiiaFn,
                                      string_object));
    string_object->SetProperty(
        MAG_CONTAINS,
        MaDiiaNative::Create(M, MAG_CONTAINS, MaText_MagContainsNativeDiiaFn,
                             string_object));
    string_object->SetProperty(
        MAG_GET_ELEMENT,
        MaDiiaNative::Create(M, MAG_GET_ELEMENT,
                             MaText_MagGetElementNativeDiiaFn, string_object));
    string_object->SetProperty(
        MAG_ITERATOR,
        MaDiiaNative::Create(M, MAG_ITERATOR, MaText_MagIteratorNativeDiiaFn,
                             string_object));
    string_object->SetProperty(
        MAG_NUMBER,
        MaDiiaNative::Create(M, MAG_NUMBER, MaText_MagNumberNativeDiiaFn,
                             string_object));
    return string_object;
  }

  MaCell MaText_Structure_MagCallNativeDiiaFn(MaMa* M,
                                              MaObject* o,
                                              MaArgs* args,
                                              const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.IsNumber()) {
      return MaCell::Object(
          MaText::Create(M, ma_number_to_string(cell.v.number)));
    }
    if (cell.IsYes()) {
      return MaCell::Object(MaText::Create(M, "так"));
    }
    if (cell.IsNo()) {
      return MaCell::Object(MaText::Create(M, "ні"));
    }
    if (cell.IsObject()) {
      if (cell.IsObjectText()) {
        return cell;
      } else if (cell.AsObject()->HasProperty(MAG_TEXT)) {
        return cell.AsObject()->GetProperty(MAG_TEXT).Call(M, {}, {});
      }
    }
    RETURN_ERROR(new MaError(
        MaCell::Object(MaText::Create(M, "Неможливо перетворити на текст.")),
        location));
  }

  void MaText::Init(MaMa* M) {
    const auto text_structure_object = MaStructure::Create(M, "текст");
    M->global_scope->SetSubject("текст", text_structure_object);
    M->text_structure_object = text_structure_object;
    text_structure_object->SetProperty(
        MAG_CALL,
        MaDiiaNative::Create(M, MAG_CALL, MaText_Structure_MagCallNativeDiiaFn,
                             text_structure_object));
  }
} // namespace mavka::mama