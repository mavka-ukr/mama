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

  size_t MaObject::textGetLength() const {
    return utf8_len(this->textData);
  }

  std::string MaObject::textSubstr(size_t start, size_t length) const {
    return utf8_substr(this->textData, start, length);
  }

  // розбити
  MaValue MaText_SplitNativeDiiaFn(MaMa* M,
                                   MaObject* scope,
                                   MaObject* diiaObject,
                                   MaObject* args,
                                   size_t li) {
    const auto cell = args->getArg(M, "0", "роздільник");
    if (cell.isObject() && cell.asObject()->isText(M)) {
      const auto delim = cell.asObject()->textData;

      if (delim.empty()) {
        const auto listObject = MaObject::CreateList(M);
        for (const auto& c :
             utf8_chars(diiaObject->diiaGetBoundObject()->textData)) {
          listObject->listAppend(M,
                                 MaValue::Object(MaObject::CreateText(M, c)));
        }
        return MaValue::Object(listObject);
      }

      const auto listObject = MaObject::CreateList(M);
      std::string current;
      for (const auto& c :
           utf8_chars(diiaObject->diiaGetBoundObject()->textData)) {
        if (c == delim) {
          listObject->listAppend(
              M, MaValue::Object(MaObject::CreateText(M, current)));
          current.clear();
        } else {
          current += c;
        }
      }
      listObject->listAppend(M,
                             MaValue::Object(MaObject::CreateText(M, current)));
      return MaValue::Object(listObject);
    } else {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaObject::CreateText(
              M, "Для дії \"розбити\" потрібен текстовий аргумент.")),
          li));
    }
  }

  // замінити
  MaValue MaText_ReplaceNativeDiiaFn(MaMa* M,
                                     MaObject* scope,
                                     MaObject* diiaObject,
                                     MaObject* args,
                                     size_t li) {
    const auto oldVal = args->getArg(M, "0", "старе");
    if (!(oldVal.isObject() && oldVal.asObject()->isText(M))) {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaObject::CreateText(
              M, "Для дії \"замінити\" перший аргумент повинен бути текстом.")),
          li));
    }
    const auto newVal = args->getArg(M, "1", "нове");
    if (!(newVal.isObject() && newVal.asObject()->isText(M))) {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaObject::CreateText(
              M, "Для дії \"замінити\" другий аргумент повинен бути текстом.")),
          li));
    }
    const auto first_string = oldVal.asObject()->textData;
    const auto second_string = newVal.asObject()->textData;
    std::string new_string;
    for (std::size_t i = 0;
         i < diiaObject->diiaGetBoundObject()->textGetLength(); i++) {
      const auto substr = diiaObject->diiaGetBoundObject()->textSubstr(i, 1);
      if (substr == first_string) {
        new_string += second_string;
      } else {
        new_string += substr;
      }
    }
    return MaValue::Object(MaObject::CreateText(M, new_string));
  }

  // починається
  MaValue MaText_StartsWithNativeDiiaFn(MaMa* M,
                                        MaObject* scope,
                                        MaObject* diiaObject,
                                        MaObject* args,
                                        size_t li) {
    const auto value = args->getArg(M, "0", "значення");
    if (value.isObject() && value.asObject()->isText(M)) {
      if (diiaObject->diiaGetBoundObject()->textData.find(
              value.asObject()->textData) == 0) {
        return MaValue::Yes();
      }
      return MaValue::No();
    } else {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaObject::CreateText(
              M, "Для дії \"починається\" потрібен текстовий аргумент.")),
          li));
    }
  }

  // закінчується
  MaValue MaText_EndsWithNativeDiiaFn(MaMa* M,
                                      MaObject* scope,
                                      MaObject* diiaObject,
                                      MaObject* args,
                                      size_t li) {
    const auto value = args->getArg(M, "0", "значення");
    if (value.isObject() && value.asObject()->isText(M)) {
      if (diiaObject->diiaGetBoundObject()->textGetLength() <
          value.asObject()->textGetLength()) {
        return MaValue::No();
      }
      if (diiaObject->diiaGetBoundObject()->textSubstr(
              diiaObject->diiaGetBoundObject()->textGetLength() -
                  value.asObject()->textGetLength(),
              value.asObject()->textGetLength()) ==
          value.asObject()->textData) {
        return MaValue::Yes();
      }
      return MaValue::No();
    } else {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaObject::CreateText(
              M, "Для дії \"закінчується\" потрібен текстовий аргумент.")),
          li));
    }
  }

  // обтяти
  MaValue MaText_TrimNativeDiiaFn(MaMa* M,
                                  MaObject* scope,
                                  MaObject* diiaObject,
                                  MaObject* args,
                                  size_t li) {
    return MaValue::Object(MaObject::CreateText(
        M, internal::tools::trim(diiaObject->diiaGetBoundObject()->textData)));
  }

  // чародія_додати
  MaValue MaText_MagAddNativeDiiaFn(MaMa* M,
                                    MaObject* scope,
                                    MaObject* diiaObject,
                                    MaObject* args,
                                    size_t li) {
    const auto arg_cell = args->getArg(M, "0", "значення");
    if (arg_cell.isEmpty()) {
      return MaValue::Object(MaObject::CreateText(
          M, diiaObject->diiaGetBoundObject()->textData + "пусто"));
    }
    if (arg_cell.isNumber()) {
      return MaValue::Object(
          MaObject::CreateText(M, diiaObject->diiaGetBoundObject()->textData +
                                      ma_number_to_string(arg_cell.v.number)));
    }
    if (arg_cell.isYes()) {
      return MaValue::Object(MaObject::CreateText(
          M, diiaObject->diiaGetBoundObject()->textData + "так"));
    }
    if (arg_cell.isNo()) {
      return MaValue::Object(MaObject::CreateText(
          M, diiaObject->diiaGetBoundObject()->textData + "ні"));
    }
    if (arg_cell.isObject()) {
      if (arg_cell.asObject()->isText(M)) {
        return MaValue::Object(
            MaObject::CreateText(M, diiaObject->diiaGetBoundObject()->textData +
                                        arg_cell.asObject()->textData));
      }
    }
    return MaValue::Error(
        MaError::Create(MaValue::Object(MaObject::CreateText(
                            M, "Неможливо додати до тексту обʼєкт типу \"" +
                                   arg_cell.getName() + "\".")),
                        li));
  }

  // чародія_містить
  MaValue MaText_MagContainsNativeDiiaFn(MaMa* M,
                                         MaObject* scope,
                                         MaObject* diiaObject,
                                         MaObject* args,
                                         size_t li) {
    const auto cell = args->getArg(M, "0", "значення");
    if (cell.isObject() && cell.asObject()->isText(M)) {
      if (diiaObject->diiaGetBoundObject()->textData.find(
              cell.asObject()->textData) != std::string::npos) {
        return MaValue::Yes();
      }
      return MaValue::No();
    } else {
      return MaValue::Error(MaError::Create(
          MaValue::Object(MaObject::CreateText(
              M, "Для дії \"чародія_містить\" потрібен текстовий аргумент.")),
          li));
    }
  }

  // чародія_отримати
  MaValue MaText_MagGetElementNativeDiiaFn(MaMa* M,
                                           MaObject* scope,
                                           MaObject* diiaObject,
                                           MaObject* args,
                                           size_t li) {
    const auto cell = args->getArg(M, "0", "позиція");
    if (cell.isNumber()) {
      const auto i = cell.asNumber();
      if (i < diiaObject->diiaGetBoundObject()->textGetLength()) {
        const auto substr = diiaObject->diiaGetBoundObject()->textSubstr(i, 1);
        return MaValue::Object(MaObject::CreateText(M, substr));
      }
    }
    return MaValue::Empty();
  }

  // чародія_перебір
  MaValue MaText_MagIteratorNativeDiiaFn(MaMa* M,
                                         MaObject* scope,
                                         MaObject* diiaObject,
                                         MaObject* args,
                                         size_t li) {
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaObject::CreateText(
            M,
            "Дія \"" + std::string(MAG_ITERATOR) + "\" тимчасово недоступна.")),
        li));
  }

  // чародія_число
  MaValue MaText_MagNumberNativeDiiaFn(MaMa* M,
                                       MaObject* scope,
                                       MaObject* diiaObject,
                                       MaObject* args,
                                       size_t li) {
    // треба вручну переписати для підтримки 0шАБВ і 0б01010110101
    return MaValue::Number(
        std::stod(diiaObject->diiaGetBoundObject()->textData));
  }

  MaObject* MaObject::CreateText(MaMa* M, const std::string& value) {
    const auto textObject = MaObject::Instance(M, M->text_structure_object);
    textObject->textData = value;
    textObject->setProperty(
        M, "розбити",
        MaObject::CreateDiiaNativeFn(M, "розбити", MaText_SplitNativeDiiaFn,
                                     textObject));
    textObject->setProperty(
        M, "замінити",
        MaObject::CreateDiiaNativeFn(M, "замінити", MaText_ReplaceNativeDiiaFn,
                                     textObject));
    textObject->setProperty(
        M, "починається",
        MaObject::CreateDiiaNativeFn(
            M, "починається", MaText_StartsWithNativeDiiaFn, textObject));
    textObject->setProperty(
        M, "закінчується",
        MaObject::CreateDiiaNativeFn(M, "закінчується",
                                     MaText_EndsWithNativeDiiaFn, textObject));
    textObject->setProperty(
        M, "обтяти",
        MaObject::CreateDiiaNativeFn(M, "обтяти", MaText_TrimNativeDiiaFn,
                                     textObject));
    textObject->setProperty(
        M, MAG_ADD,
        MaObject::CreateDiiaNativeFn(M, MAG_ADD, MaText_MagAddNativeDiiaFn,
                                     textObject));
    textObject->setProperty(
        M, MAG_CONTAINS,
        MaObject::CreateDiiaNativeFn(
            M, MAG_CONTAINS, MaText_MagContainsNativeDiiaFn, textObject));
    textObject->setProperty(
        M, MAG_GET_ELEMENT,
        MaObject::CreateDiiaNativeFn(
            M, MAG_GET_ELEMENT, MaText_MagGetElementNativeDiiaFn, textObject));
    textObject->setProperty(
        M, MAG_ITERATOR,
        MaObject::CreateDiiaNativeFn(
            M, MAG_ITERATOR, MaText_MagIteratorNativeDiiaFn, textObject));
    textObject->setProperty(
        M, MAG_NUMBER,
        MaObject::CreateDiiaNativeFn(M, MAG_NUMBER,
                                     MaText_MagNumberNativeDiiaFn, textObject));
    return textObject;
  }

  MaValue MaText_Structure_MagCallNativeDiiaFn(MaMa* M,
                                               MaObject* scope,
                                               MaObject* diiaObject,
                                               MaObject* args,
                                               size_t li) {
    const auto cell = args->getArg(M, "0", "значення");
    if (cell.isNumber()) {
      return MaValue::Object(
          MaObject::CreateText(M, ma_number_to_string(cell.v.number)));
    }
    if (cell.isYes()) {
      return MaValue::Object(MaObject::CreateText(M, "так"));
    }
    if (cell.isNo()) {
      return MaValue::Object(MaObject::CreateText(M, "ні"));
    }
    if (cell.isObject()) {
      if (cell.asObject()->isText(M)) {
        return cell;
      }
      return cell.asObject()->getProperty(M, MAG_TEXT).call(M, scope, {}, {});
    }
    return MaValue::Error(
        MaError::Create(MaValue::Object(MaObject::CreateText(
                            M, "Неможливо перетворити на текст.")),
                        li));
  }

  void InitText(MaMa* M) {
    const auto textStructureObject = MaObject::CreateStructure(M, "текст");
    M->global_scope->setProperty(M, "текст", textStructureObject);
    M->text_structure_object = textStructureObject;
    textStructureObject->setProperty(
        M, MAG_CALL,
        MaObject::CreateDiiaNativeFn(M, MAG_CALL,
                                     MaText_Structure_MagCallNativeDiiaFn,
                                     textStructureObject));
  }
} // namespace mavka::mama