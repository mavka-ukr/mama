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

  void MaObject::textSetData(const std::string& data) {
    this->textData = data;
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
        const auto listObject = M->createList();
        for (const auto& c :
             utf8_chars(diiaObject->diiaGetBoundObject()->textData)) {
          listObject->listAppend(M, MaValue::Object(M->createText(c)));
        }
        return MaValue::Object(listObject);
      }

      const auto listObject = M->createList();
      std::string current;
      for (const auto& c :
           utf8_chars(diiaObject->diiaGetBoundObject()->textData)) {
        if (c == delim) {
          listObject->listAppend(M, MaValue::Object(M->createText(current)));
          current.clear();
        } else {
          current += c;
        }
      }
      listObject->listAppend(M, MaValue::Object(M->createText(current)));
      return MaValue::Object(listObject);
    } else {
      return MaValue::Error(MaError::Create(
          MaValue::Object(M->createText(
              "Для дії \"розбити\" потрібен текстовий аргумент.")),
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
          MaValue::Object(M->createText(
              "Для дії \"замінити\" перший аргумент повинен бути текстом.")),
          li));
    }
    const auto newVal = args->getArg(M, "1", "нове");
    if (!(newVal.isObject() && newVal.asObject()->isText(M))) {
      return MaValue::Error(MaError::Create(
          MaValue::Object(M->createText(
              "Для дії \"замінити\" другий аргумент повинен бути текстом.")),
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
    return MaValue::Object(M->createText(new_string));
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
          MaValue::Object(M->createText(
              "Для дії \"починається\" потрібен текстовий аргумент.")),
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
          MaValue::Object(M->createText(
              "Для дії \"закінчується\" потрібен текстовий аргумент.")),
          li));
    }
  }

  // обтяти
  MaValue MaText_TrimNativeDiiaFn(MaMa* M,
                                  MaObject* scope,
                                  MaObject* diiaObject,
                                  MaObject* args,
                                  size_t li) {
    return MaValue::Object(M->createText(
        internal::tools::trim(diiaObject->diiaGetBoundObject()->textData)));
  }

  // чародія_додати
  MaValue MaText_MagAddNativeDiiaFn(MaMa* M,
                                    MaObject* scope,
                                    MaObject* diiaObject,
                                    MaObject* args,
                                    size_t li) {
    const auto arg_cell = args->getArg(M, "0", "значення");
    if (arg_cell.isEmpty()) {
      return MaValue::Object(
          M->createText(diiaObject->diiaGetBoundObject()->textData + "пусто"));
    }
    if (arg_cell.isNumber()) {
      return MaValue::Object(
          M->createText(diiaObject->diiaGetBoundObject()->textData +
                        ma_number_to_string(arg_cell.v.number)));
    }
    if (arg_cell.isYes()) {
      return MaValue::Object(
          M->createText(diiaObject->diiaGetBoundObject()->textData + "так"));
    }
    if (arg_cell.isNo()) {
      return MaValue::Object(
          M->createText(diiaObject->diiaGetBoundObject()->textData + "ні"));
    }
    if (arg_cell.isObject()) {
      if (arg_cell.asObject()->isText(M)) {
        return MaValue::Object(
            M->createText(diiaObject->diiaGetBoundObject()->textData +
                          arg_cell.asObject()->textData));
      }
    }
    return MaValue::Error(MaError::Create(
        MaValue::Object(
            M->createText("Неможливо додати до тексту обʼєкт типу \"" +
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
          MaValue::Object(M->createText(
              "Для дії \"чародія_містить\" потрібен текстовий аргумент.")),
          li));
    }
  }

  // чародія_отримати
  MaValue MaText_MagGetElementNativeDiiaFn(MaMa* M,
                                           MaObject* scope,
                                           MaObject* diiaObject,
                                           MaObject* args,
                                           size_t li) {
    const auto positionValue = args->getArg(M, "0", "позиція");
    if (positionValue.isNumber()) {
      const auto i = positionValue.asNumber();
      if (i < diiaObject->diiaGetBoundObject()->textGetLength()) {
        const auto substr = diiaObject->diiaGetBoundObject()->textSubstr(i, 1);
        return MaValue::Object(M->createText(substr));
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
        MaValue::Object(M->createText("Дія \"" + std::string(MAG_ITERATOR) +
                                      "\" тимчасово недоступна.")),
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

  MaValue MaText_Structure_MagCallNativeDiiaFn(MaMa* M,
                                               MaObject* scope,
                                               MaObject* diiaObject,
                                               MaObject* args,
                                               size_t li) {
    const auto cell = args->getArg(M, "0", "значення");
    if (cell.isNumber()) {
      return MaValue::Object(M->createText(ma_number_to_string(cell.v.number)));
    }
    if (cell.isYes()) {
      return MaValue::Object(M->createText("так"));
    }
    if (cell.isNo()) {
      return MaValue::Object(M->createText("ні"));
    }
    if (cell.isObject()) {
      if (cell.asObject()->isText(M)) {
        return cell;
      }
      return cell.asObject()->getProperty(M, MAG_TEXT).call(M, scope, {}, {});
    }
    return MaValue::Error(MaError::Create(
        MaValue::Object(M->createText("Неможливо перетворити на текст.")), li));
  }

  void InitText(MaMa* M) {
    const auto textStructureObject = M->createStructure("текст");
    M->global_scope->setProperty(M, "текст", textStructureObject);
    M->text_structure_object = textStructureObject;
    textStructureObject->setProperty(
        M, MAG_CALL,
        M->createNativeDiia(MAG_CALL, MaText_Structure_MagCallNativeDiiaFn,
                            textStructureObject));
    textStructureObject->structurePushMethod(
        M->createNativeDiia(MAG_ADD, MaText_MagAddNativeDiiaFn, nullptr));
  }
} // namespace mavka::mama