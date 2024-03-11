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

  size_t MaText::GetLength() const {
    return utf8_len(this->data);
  }

  std::string MaText::Substr(size_t start, size_t length) const {
    return utf8_substr(this->data, start, length);
  }

  // розбити
  MaValue MaText_SplitNativeDiiaFn(MaMa* M,
                                   MaObject* native_o,
                                   MaArgs* args,
                                   const MaLocation& location) {
    const auto cell = args->Get(0, "роздільник");
    if (cell.IsObject() && cell.IsObjectText()) {
      const auto delim = cell.AsObject()->AsText()->data;

      if (delim.empty()) {
        const auto list_o = MaList::Create(M);
        for (const auto& c :
             utf8_chars(native_o->AsNative()->GetMe()->AsText()->data)) {
          list_o->AsList()->Append(MaValue::Object(MaText::Create(M, c)));
        }
        return MaValue::Object(list_o);
      }

      const auto list_o = MaList::Create(M);
      std::string current;
      for (const auto& c :
           utf8_chars(native_o->AsNative()->GetMe()->AsText()->data)) {
        if (c == delim) {
          list_o->AsList()->Append(MaValue::Object(MaText::Create(M, current)));
          current.clear();
        } else {
          current += c;
        }
      }
      list_o->AsList()->Append(MaValue::Object(MaText::Create(M, current)));
      return MaValue::Object(list_o);
    } else {
      return MaValue::Error(new MaError(MaValue::Object(MaText::Create(
          M, "Для дії \"розбити\" потрібен текстовий аргумент."))));
    }
  }

  // замінити
  MaValue MaText_ReplaceNativeDiiaFn(MaMa* M,
                                     MaObject* o,
                                     MaArgs* args,
                                     const MaLocation& location) {
    const auto oldVal = args->Get(0, "старе");
    if (!(oldVal.IsObject() && oldVal.IsObjectText())) {
      return MaValue::Error(new MaError(MaValue::Object(MaText::Create(
          M, "Для дії \"замінити\" перший аргумент повинен бути текстом."))));
    }
    const auto newVal = args->Get(1, "нове");
    if (!(newVal.IsObject() && newVal.IsObjectText())) {
      return MaValue::Error(new MaError(MaValue::Object(MaText::Create(
          M, "Для дії \"замінити\" другий аргумент повинен бути текстом."))));
    }
    const auto first_string = oldVal.AsText()->data;
    const auto second_string = newVal.AsText()->data;
    std::string new_string;
    for (std::size_t i = 0; i < o->d.native->me->d.text->GetLength(); i++) {
      const auto substr = o->d.native->me->d.text->Substr(i, 1);
      if (substr == first_string) {
        new_string += second_string;
      } else {
        new_string += substr;
      }
    }
    return MaValue::Object(MaText::Create(M, new_string));
  }

  // починається
  MaValue MaText_StartsWithNativeDiiaFn(MaMa* M,
                                        MaObject* o,
                                        MaArgs* args,
                                        const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.IsObject() && cell.IsObjectText()) {
      if (o->AsNative()->GetMe()->AsText()->data.find(
              cell.AsObject()->AsText()->data) == 0) {
        return MaValue::Yes();
      }
      return MaValue::No();
    } else {
      return MaValue::Error(new MaError(MaValue::Object(MaText::Create(
          M, "Для дії \"починається\" потрібен текстовий аргумент."))));
    }
  }

  // закінчується
  MaValue MaText_EndsWithNativeDiiaFn(MaMa* M,
                                      MaObject* o,
                                      MaArgs* args,
                                      const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.IsObject() && cell.IsObjectText()) {
      const auto text = cell.AsText();
      if (o->AsNative()->GetMe()->AsText()->GetLength() < text->GetLength()) {
        return MaValue::No();
      }
      if (o->AsNative()->GetMe()->AsText()->Substr(
              o->AsNative()->GetMe()->AsText()->GetLength() - text->GetLength(),
              text->GetLength()) == text->data) {
        return MaValue::Yes();
      }
      return MaValue::No();
    } else {
      return MaValue::Error(new MaError(MaValue::Object(MaText::Create(
          M, "Для дії \"закінчується\" потрібен текстовий аргумент."))));
    }
  }

  // обтяти
  MaValue MaText_TrimNativeDiiaFn(MaMa* M,
                                  MaObject* o,
                                  MaArgs* args,
                                  const MaLocation& location) {
    return MaValue::Object(MaText::Create(
        M, internal::tools::trim(o->AsNative()->GetMe()->AsText()->data)));
  }

  // чародія_додати
  MaValue MaText_MagAddNativeDiiaFn(MaMa* M,
                                    MaObject* o,
                                    MaArgs* args,
                                    const MaLocation& location) {
    const auto arg_cell = args->Get(0, "значення");
    if (arg_cell.IsEmpty()) {
      return MaValue::Object(
          MaText::Create(M, o->AsNative()->GetMe()->AsText()->data + "пусто"));
    }
    if (arg_cell.IsNumber()) {
      return MaValue::Object(
          MaText::Create(M, o->AsNative()->GetMe()->AsText()->data +
                                ma_number_to_string(arg_cell.v.number)));
    }
    if (arg_cell.IsYes()) {
      return MaValue::Object(
          MaText::Create(M, o->AsNative()->GetMe()->AsText()->data + "так"));
    }
    if (arg_cell.IsNo()) {
      return MaValue::Object(
          MaText::Create(M, o->AsNative()->GetMe()->AsText()->data + "ні"));
    }
    if (arg_cell.IsObject()) {
      if (arg_cell.IsObjectText()) {
        return MaValue::Object(
            MaText::Create(M, o->AsNative()->GetMe()->AsText()->data +
                                  arg_cell.AsObject()->AsText()->data));
      }
    }
    return MaValue::Error(new MaError(MaValue::Object(
        MaText::Create(M, "Неможливо додати до тексту обʼєкт типу \"" +
                              arg_cell.GetName() + "\"."))));
  }

  // чародія_містить
  MaValue MaText_MagContainsNativeDiiaFn(MaMa* M,
                                         MaObject* o,
                                         MaArgs* args,
                                         const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.IsObject() && cell.IsObjectText()) {
      if (o->AsNative()->GetMe()->AsText()->data.find(
              cell.AsObject()->AsText()->data) != std::string::npos) {
        return MaValue::Yes();
      }
      return MaValue::No();
    } else {
      return MaValue::Error(new MaError(MaValue::Object(MaText::Create(
          M, "Для дії \"чародія_містить\" потрібен текстовий аргумент."))));
    }
  }

  // чародія_отримати
  MaValue MaText_MagGetElementNativeDiiaFn(MaMa* M,
                                           MaObject* o,
                                           MaArgs* args,
                                           const MaLocation& location) {
    const auto cell = args->Get(0, "позиція");
    if (cell.IsNumber()) {
      const auto i = cell.AsNumber();
      if (i < o->AsNative()->GetMe()->AsText()->GetLength()) {
        const auto substr = o->AsNative()->GetMe()->AsText()->Substr(i, 1);
        return MaValue::Object(MaText::Create(M, substr));
      }
    }
    return MaValue::Empty();
  }

  // чародія_перебір
  MaValue MaText_MagIteratorNativeDiiaFn(MaMa* M,
                                         MaObject* o,
                                         MaArgs* args,
                                         const MaLocation& location) {
    return MaValue::Error(new MaError(
        MaValue::Object(MaText::Create(M, "Дія \"" + std::string(MAG_ITERATOR) +
                                              "\" тимчасово недоступна."))));
  }

  // чародія_число
  MaValue MaText_MagNumberNativeDiiaFn(MaMa* M,
                                       MaObject* o,
                                       MaArgs* args,
                                       const MaLocation& location) {
    // треба вручну переписати для підтримки 0шАБВ і 0б01010110101
    return MaValue::Number(std::stod(o->d.native->me->d.text->data));
  }

  MaValue MaText_GetHandler(MaMa* M, MaObject* o, const std::string& name) {
    if (name == "довжина") {
      return MaValue::Number(o->AsText()->GetLength());
    }
    return o->GetPropertyStrongDirect(M, name);
  }

  MaObject* MaText::Create(MaMa* M, const std::string& value) {
    const auto string = new MaText();
    string->data = value;
    const auto text_o = MaObject::Instance(M, MA_OBJECT_STRING,
                                           M->text_structure_object, string);
    text_o->get = MaText_GetHandler;
    text_o->SetProperty(
        M, "розбити",
        MaNative::Create(M, "розбити", MaText_SplitNativeDiiaFn, text_o));
    text_o->SetProperty(
        M, "замінити",
        MaNative::Create(M, "замінити", MaText_ReplaceNativeDiiaFn, text_o));
    text_o->SetProperty(
        M, "починається",
        MaNative::Create(M, "починається", MaText_StartsWithNativeDiiaFn,
                         text_o));
    text_o->SetProperty(M, "закінчується",
                        MaNative::Create(M, "закінчується",
                                         MaText_EndsWithNativeDiiaFn, text_o));
    text_o->SetProperty(
        M, "обтяти",
        MaNative::Create(M, "обтяти", MaText_TrimNativeDiiaFn, text_o));
    text_o->SetProperty(
        M, MAG_ADD,
        MaNative::Create(M, MAG_ADD, MaText_MagAddNativeDiiaFn, text_o));
    text_o->SetProperty(
        M, MAG_CONTAINS,
        MaNative::Create(M, MAG_CONTAINS, MaText_MagContainsNativeDiiaFn,
                         text_o));
    text_o->SetProperty(
        M, MAG_GET_ELEMENT,
        MaNative::Create(M, MAG_GET_ELEMENT, MaText_MagGetElementNativeDiiaFn,
                         text_o));
    text_o->SetProperty(
        M, MAG_ITERATOR,
        MaNative::Create(M, MAG_ITERATOR, MaText_MagIteratorNativeDiiaFn,
                         text_o));
    text_o->SetProperty(
        M, MAG_NUMBER,
        MaNative::Create(M, MAG_NUMBER, MaText_MagNumberNativeDiiaFn, text_o));
    return text_o;
  }

  MaValue MaText_Structure_MagCallNativeDiiaFn(MaMa* M,
                                               MaObject* native_o,
                                               MaArgs* args,
                                               const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.IsNumber()) {
      return MaValue::Object(
          MaText::Create(M, ma_number_to_string(cell.v.number)));
    }
    if (cell.IsYes()) {
      return MaValue::Object(MaText::Create(M, "так"));
    }
    if (cell.IsNo()) {
      return MaValue::Object(MaText::Create(M, "ні"));
    }
    if (cell.IsObject()) {
      if (cell.IsObjectText()) {
        return cell;
      }
      return cell.AsObject()->GetPropertyStrong(M, MAG_TEXT).Call(M, {}, {});
    }
    return MaValue::Error(new MaError(
        MaValue::Object(MaText::Create(M, "Неможливо перетворити на текст.")),
        location));
  }

  void MaText::Init(MaMa* M) {
    const auto text_structure_object = MaStructure::Create(M, "текст");
    M->global_scope->SetSubject("текст", text_structure_object);
    M->text_structure_object = text_structure_object;
    text_structure_object->SetProperty(
        M, MAG_CALL,
        MaNative::Create(M, MAG_CALL, MaText_Structure_MagCallNativeDiiaFn,
                         text_structure_object));
  }
} // namespace mavka::mama