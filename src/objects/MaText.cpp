#include <csetjmp>

#include "../external/utf8/utf8.h"
#include "../mama.h"

// TODO: переписати це все

namespace mavka::mama {
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

  size_t MaText::getLength() const {
    return utf8_len(this->data);
  }

  std::string MaText::Substr(size_t start, size_t length) const {
    return utf8_substr(this->data, start, length);
  }

  // розбити
  MaValue MaText_SplitNativeDiiaFn(MaMa* M,
                                   MaObject* native_o,
                                   MaObject* args,
                                   const MaLocation& location) {
    const auto cell = args->getArg(M, "0", "роздільник");
    if (cell.isObject() && cell.asObject()->isText(M)) {
      const auto delim = cell.asObject()->asText()->data;

      if (delim.empty()) {
        const auto list_o = MaList::Create(M);
        for (const auto& c :
             utf8_chars(native_o->asDiia()->getMe()->asText()->data)) {
          list_o->asList()->append(M, MaValue::Object(MaText::Create(M, c)));
        }
        return MaValue::Object(list_o);
      }

      const auto list_o = MaList::Create(M);
      std::string current;
      for (const auto& c :
           utf8_chars(native_o->asDiia()->getMe()->asText()->data)) {
        if (c == delim) {
          list_o->asList()->append(M,
                                   MaValue::Object(MaText::Create(M, current)));
          current.clear();
        } else {
          current += c;
        }
      }
      list_o->asList()->append(M, MaValue::Object(MaText::Create(M, current)));
      return MaValue::Object(list_o);
    } else {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaText::Create(
              M, "Для дії \"розбити\" потрібен текстовий аргумент.")),
          M->call_stack.top()->module, location));
    }
  }

  // замінити
  MaValue MaText_ReplaceNativeDiiaFn(MaMa* M,
                                     MaObject* o,
                                     MaObject* args,
                                     const MaLocation& location) {
    const auto oldVal = args->getArg(M, "0", "старе");
    if (!(oldVal.isObject() && oldVal.asObject()->isText(M))) {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaText::Create(
              M, "Для дії \"замінити\" перший аргумент повинен бути текстом.")),
          M->call_stack.top()->module, location));
    }
    const auto newVal = args->getArg(M, "1", "нове");
    if (!(newVal.isObject() && newVal.asObject()->isText(M))) {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaText::Create(
              M, "Для дії \"замінити\" другий аргумент повинен бути текстом.")),
          M->call_stack.top()->module, location));
    }
    const auto first_string = oldVal.asText()->data;
    const auto second_string = newVal.asText()->data;
    std::string new_string;
    for (std::size_t i = 0; i < o->d.diia->me->d.text->getLength(); i++) {
      const auto substr = o->d.diia->me->d.text->Substr(i, 1);
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
                                        MaObject* args,
                                        const MaLocation& location) {
    const auto cell = args->getArg(M, "0", "значення");
    if (cell.isObject() && cell.asObject()->isText(M)) {
      if (o->asDiia()->getMe()->asText()->data.find(
              cell.asObject()->asText()->data) == 0) {
        return MaValue::Yes();
      }
      return MaValue::No();
    } else {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaText::Create(
              M, "Для дії \"починається\" потрібен текстовий аргумент.")),
          M->call_stack.top()->module, location));
    }
  }

  // закінчується
  MaValue MaText_EndsWithNativeDiiaFn(MaMa* M,
                                      MaObject* o,
                                      MaObject* args,
                                      const MaLocation& location) {
    const auto cell = args->getArg(M, "0", "значення");
    if (cell.isObject() && cell.asObject()->isText(M)) {
      const auto text = cell.asText();
      if (o->asDiia()->getMe()->asText()->getLength() < text->getLength()) {
        return MaValue::No();
      }
      if (o->asDiia()->getMe()->asText()->Substr(
              o->asDiia()->getMe()->asText()->getLength() - text->getLength(),
              text->getLength()) == text->data) {
        return MaValue::Yes();
      }
      return MaValue::No();
    } else {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaText::Create(
              M, "Для дії \"закінчується\" потрібен текстовий аргумент.")),
          M->call_stack.top()->module, location));
    }
  }

  // обтяти
  MaValue MaText_TrimNativeDiiaFn(MaMa* M,
                                  MaObject* o,
                                  MaObject* args,
                                  const MaLocation& location) {
    return MaValue::Object(MaText::Create(
        M, internal::tools::trim(o->asDiia()->getMe()->asText()->data)));
  }

  // чародія_додати
  MaValue MaText_MagAddNativeDiiaFn(MaMa* M,
                                    MaObject* o,
                                    MaObject* args,
                                    const MaLocation& location) {
    const auto arg_cell = args->getArg(M, "0", "значення");
    if (arg_cell.isEmpty()) {
      return MaValue::Object(
          MaText::Create(M, o->asDiia()->getMe()->asText()->data + "пусто"));
    }
    if (arg_cell.isNumber()) {
      return MaValue::Object(
          MaText::Create(M, o->asDiia()->getMe()->asText()->data +
                                ma_number_to_string(arg_cell.v.number)));
    }
    if (arg_cell.isYes()) {
      return MaValue::Object(
          MaText::Create(M, o->asDiia()->getMe()->asText()->data + "так"));
    }
    if (arg_cell.isNo()) {
      return MaValue::Object(
          MaText::Create(M, o->asDiia()->getMe()->asText()->data + "ні"));
    }
    if (arg_cell.isObject()) {
      if (arg_cell.asObject()->isText(M)) {
        return MaValue::Object(
            MaText::Create(M, o->asDiia()->getMe()->asText()->data +
                                  arg_cell.asObject()->asText()->data));
      }
    }
    return MaValue::Error(
        MaError::Create(MaValue::Object(MaText::Create(
                            M, "Неможливо додати до тексту обʼєкт типу \"" +
                                   arg_cell.getName() + "\".")),
                        M->call_stack.top()->module, location));
  }

  // чародія_містить
  MaValue MaText_MagContainsNativeDiiaFn(MaMa* M,
                                         MaObject* o,
                                         MaObject* args,
                                         const MaLocation& location) {
    const auto cell = args->getArg(M, "0", "значення");
    if (cell.isObject() && cell.asObject()->isText(M)) {
      if (o->asDiia()->getMe()->asText()->data.find(
              cell.asObject()->asText()->data) != std::string::npos) {
        return MaValue::Yes();
      }
      return MaValue::No();
    } else {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaText::Create(
              M, "Для дії \"чародія_містить\" потрібен текстовий аргумент.")),
          M->call_stack.top()->module, location));
    }
  }

  // чародія_отримати
  MaValue MaText_MagGetElementNativeDiiaFn(MaMa* M,
                                           MaObject* o,
                                           MaObject* args,
                                           const MaLocation& location) {
    const auto cell = args->getArg(M, "0", "позиція");
    if (cell.isNumber()) {
      const auto i = cell.asNumber();
      if (i < o->asDiia()->getMe()->asText()->getLength()) {
        const auto substr = o->asDiia()->getMe()->asText()->Substr(i, 1);
        return MaValue::Object(MaText::Create(M, substr));
      }
    }
    return MaValue::Empty();
  }

  // чародія_перебір
  MaValue MaText_MagIteratorNativeDiiaFn(MaMa* M,
                                         MaObject* o,
                                         MaObject* args,
                                         const MaLocation& location) {
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaText::Create(M, "Дія \"" + std::string(MAG_ITERATOR) +
                                              "\" тимчасово недоступна.")),
        M->call_stack.top()->module, location));
  }

  // чародія_число
  MaValue MaText_MagNumberNativeDiiaFn(MaMa* M,
                                       MaObject* o,
                                       MaObject* args,
                                       const MaLocation& location) {
    // треба вручну переписати для підтримки 0шАБВ і 0б01010110101
    return MaValue::Number(std::stod(o->d.diia->me->d.text->data));
  }

  MaObject* MaText::Create(MaMa* M, const std::string& value) {
    const auto string = new MaText();
    string->data = value;
    const auto text_o = MaObject::Instance(M, M->text_structure_object, string);
    text_o->setProperty(
        M, "розбити",
        MaDiia::Create(M, "розбити", MaText_SplitNativeDiiaFn, text_o));
    text_o->setProperty(
        M, "замінити",
        MaDiia::Create(M, "замінити", MaText_ReplaceNativeDiiaFn, text_o));
    text_o->setProperty(M, "починається",
                        MaDiia::Create(M, "починається",
                                       MaText_StartsWithNativeDiiaFn, text_o));
    text_o->setProperty(
        M, "закінчується",
        MaDiia::Create(M, "закінчується", MaText_EndsWithNativeDiiaFn, text_o));
    text_o->setProperty(
        M, "обтяти",
        MaDiia::Create(M, "обтяти", MaText_TrimNativeDiiaFn, text_o));
    text_o->setProperty(
        M, MAG_ADD,
        MaDiia::Create(M, MAG_ADD, MaText_MagAddNativeDiiaFn, text_o));
    text_o->setProperty(M, MAG_CONTAINS,
                        MaDiia::Create(M, MAG_CONTAINS,
                                       MaText_MagContainsNativeDiiaFn, text_o));
    text_o->setProperty(
        M, MAG_GET_ELEMENT,
        MaDiia::Create(M, MAG_GET_ELEMENT, MaText_MagGetElementNativeDiiaFn,
                       text_o));
    text_o->setProperty(M, MAG_ITERATOR,
                        MaDiia::Create(M, MAG_ITERATOR,
                                       MaText_MagIteratorNativeDiiaFn, text_o));
    text_o->setProperty(
        M, MAG_NUMBER,
        MaDiia::Create(M, MAG_NUMBER, MaText_MagNumberNativeDiiaFn, text_o));
    return text_o;
  }

  MaValue MaText_Structure_MagCallNativeDiiaFn(MaMa* M,
                                               MaObject* native_o,
                                               MaObject* args,
                                               const MaLocation& location) {
    const auto cell = args->getArg(M, "0", "значення");
    if (cell.isNumber()) {
      return MaValue::Object(
          MaText::Create(M, ma_number_to_string(cell.v.number)));
    }
    if (cell.isYes()) {
      return MaValue::Object(MaText::Create(M, "так"));
    }
    if (cell.isNo()) {
      return MaValue::Object(MaText::Create(M, "ні"));
    }
    if (cell.isObject()) {
      if (cell.asObject()->isText(M)) {
        return cell;
      }
      return cell.asObject()->getProperty(M, MAG_TEXT).call(M, {}, {});
    }
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaText::Create(M, "Неможливо перетворити на текст.")),
        M->call_stack.top()->module, location));
  }

  void MaText::Init(MaMa* M) {
    const auto text_structure_object = MaStructure::Create(M, "текст");
    M->global_scope->setProperty(M, "текст", text_structure_object);
    M->text_structure_object = text_structure_object;
    text_structure_object->setProperty(
        M, MAG_CALL,
        MaDiia::Create(M, MAG_CALL, MaText_Structure_MagCallNativeDiiaFn,
                       text_structure_object));
  }
} // namespace mavka::mama