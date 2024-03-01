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

  size_t MaText::length() const {
    return utf8_len(this->data);
  }

  std::string MaText::substr(size_t start, size_t length) const {
    return utf8_substr(this->data, start, length);
  }

  MaCell ma_string_split_diia_native_fn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto cell = MA_ARGS_GET(args, 0, "роздільник", MA_MAKE_EMPTY());
    if (IS_STRING(cell)) {
      const auto delim = cell.v.object->d.string->data;
      if (delim.empty()) {
        const auto list_object = MaList::Create(M);
        for (const auto& c : utf8_chars(o->d.diia_native->me->d.string->data)) {
          list_object->d.list->data.push_back(
              MA_MAKE_OBJECT(MaText::Create(M, c)));
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
            MA_MAKE_OBJECT(MaText::Create(M, s)));
      }
      RETURN_OBJECT(list_object);
    } else {
      M->throw_cell = MA_MAKE_OBJECT(MaText::Create(
          M, "Для дії \"розбити\" потрібен текстовий аргумент."));
      throw MaException();
    }
  }

  MaCell ma_string_replace_diia_native_fn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto first = MA_ARGS_GET(args, 0, "старе", MA_MAKE_EMPTY());
    if (!IS_STRING(first)) {
      M->throw_cell = MA_MAKE_OBJECT(MaText::Create(
          M, "Для дії \"замінити\" перший аргумент повинен бути текстом."));
      throw MaException();
    }
    const auto second = MA_ARGS_GET(args, 1, "нове", MA_MAKE_EMPTY());
    if (!IS_STRING(second)) {
      M->throw_cell = MA_MAKE_OBJECT(MaText::Create(
          M, "Для дії \"замінити\" другий аргумент повинен бути текстом."));
      throw MaException();
    }
    const auto first_string = first.v.object->d.string->data;
    const auto second_string = second.v.object->d.string->data;
    std::string new_string;
    for (std::size_t i = 0; i < o->d.diia_native->me->d.string->length(); i++) {
      const auto substr = o->d.diia_native->me->d.string->substr(i, 1);
      if (substr == first_string) {
        new_string += second_string;
      } else {
        new_string += substr;
      }
    }
    RETURN(MA_MAKE_OBJECT(MaText::Create(M, new_string)));
  }

  MaCell ma_string_starts_with_diia_native_fn(MaMa* M,
                                              MaObject* o,
                                              MaArgs* args) {
    const auto cell = MA_ARGS_GET(args, 0, "значення", MA_MAKE_EMPTY());
    if (IS_STRING(cell)) {
      if (o->d.diia_native->me->d.string->data.find(
              cell.v.object->d.string->data) == 0) {
        RETURN_YES();
      }
      RETURN_NO();
    } else {
      M->throw_cell = MA_MAKE_OBJECT(MaText::Create(
          M, "Для дії \"починається\" потрібен текстовий аргумент."));
      throw MaException();
    }
  }

  MaCell ma_string_ends_with_diia_native_fn(MaMa* M,
                                            MaObject* o,
                                            MaArgs* args) {
    const auto cell = MA_ARGS_GET(args, 0, "значення", MA_MAKE_EMPTY());
    if (IS_STRING(cell)) {
      const auto str = cell.v.object->d.string;
      if (o->d.diia_native->me->d.string->length() < str->length()) {
        RETURN_NO();
      }
      if (o->d.diia_native->me->d.string->substr(
              o->d.diia_native->me->d.string->length() - str->length(),
              str->length()) == str->data) {
        RETURN(MA_MAKE_YES());
      }
      RETURN_NO();
    } else {
      M->throw_cell = MA_MAKE_OBJECT(MaText::Create(
          M, "Для дії \"закінчується\" потрібен текстовий аргумент."));
      throw MaException();
    }
  }

  MaCell ma_string_trim_diia_native_fn(MaMa* M, MaObject* o, MaArgs* args) {
    RETURN(MA_MAKE_OBJECT(MaText::Create(
        M, internal::tools::trim(o->d.diia_native->me->d.string->data))));
  }

  MaCell ma_string_mag_add_diia_native_fn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto arg_cell = MA_ARGS_GET(args, 0, "значення", MA_MAKE_EMPTY());
    if (IS_EMPTY(arg_cell)) {
      RETURN(MA_MAKE_OBJECT(
          MaText::Create(M, o->d.diia_native->me->d.string->data + "пусто")));
    }
    if (IS_NUMBER(arg_cell)) {
      RETURN(MA_MAKE_OBJECT(
          MaText::Create(M, o->d.diia_native->me->d.string->data +
                                ma_number_to_string(arg_cell.v.number))));
    }
    if (IS_YES(arg_cell)) {
      RETURN(MA_MAKE_OBJECT(
          MaText::Create(M, o->d.diia_native->me->d.string->data + "так")));
    }
    if (IS_NO(arg_cell)) {
      RETURN(MA_MAKE_OBJECT(
          MaText::Create(M, o->d.diia_native->me->d.string->data + "ні")));
    }
    if (IS_OBJECT(arg_cell)) {
      if (IS_OBJECT_STRING(arg_cell)) {
        RETURN(MA_MAKE_OBJECT(
            MaText::Create(M, o->d.diia_native->me->d.string->data +
                                  arg_cell.v.object->d.string->data)));
      }
    }
    M->throw_cell = MA_MAKE_OBJECT(
        MaText::Create(M, "Неможливо додати до тексту обʼєкт типу \"" +
                              arg_cell.get_name() + "\"."));
    throw MaException();
  }

  MaCell ma_string_mag_contains_diia_native_fn(MaMa* M,
                                               MaObject* o,
                                               MaArgs* args) {
    const auto cell = MA_ARGS_GET(args, 0, "значення", MA_MAKE_EMPTY());
    if (IS_STRING(cell)) {
      if (o->d.diia_native->me->d.string->data.find(
              cell.v.object->d.string->data) != std::string::npos) {
        RETURN(MA_MAKE_YES());
      }
      RETURN_NO();
    } else {
      M->throw_cell = MA_MAKE_OBJECT(MaText::Create(
          M, "Для дії \"чародія_містить\" потрібен текстовий аргумент."));
      throw MaException();
    }
  }

  MaCell ma_string_mag_get_element_diia_native_fn(MaMa* M,
                                                  MaObject* o,
                                                  MaArgs* args) {
    const auto cell = MA_ARGS_GET(args, 0, "позиція", MA_MAKE_EMPTY());
    if (IS_NUMBER(cell)) {
      const auto i = cell.v.number;
      if (i < o->d.diia_native->me->d.string->length()) {
        const auto substr = o->d.diia_native->me->d.string->substr(i, 1);
        RETURN(MA_MAKE_OBJECT(MaText::Create(M, substr)));
      }
    }
    RETURN_EMPTY();
  }

  MaCell ma_string_mag_iterator_diia_native_fn(MaMa* M,
                                               MaObject* o,
                                               MaArgs* args) {
    M->throw_cell = MA_MAKE_OBJECT(MaText::Create(
        M, "Дія \"" + std::string(MAG_ITERATOR) + "\" тимчасово недоступна."));
    throw MaException();
  }

  MaCell ma_string_mag_number_diia_native_fn(MaMa* M,
                                             MaObject* o,
                                             MaArgs* args) {
    RETURN_NUMBER(std::stod(o->d.diia_native->me->d.string->data));
  }

  MaCell ma_string_get_handler(MaMa* M, MaObject* me, const std::string& name) {
    if (name == "довжина") {
      return MA_MAKE_INTEGER(me->d.string->length());
    }
    if (!me->properties.contains(name)) {
      M->throw_cell = MA_MAKE_OBJECT(MaText::Create(
          M, "Властивість \"" + name + "\" не визначено для типу \"текст\"."));
      throw MaException();
    }
    return me->properties[name];
  }

  MaObject* MaText::Create(MaMa* M, const std::string& value) {
    const auto string = new MaText();
    string->data = value;
    const auto string_object = MaObject::Instance(
        M, MA_OBJECT_STRING, M->text_structure_object, string);
    string_object->get = ma_string_get_handler;
    ma_object_set(
        string_object, "розбити",
        MA_MAKE_OBJECT(MaDiiaNative::Create(
            M, "розбити", ma_string_split_diia_native_fn, string_object)));
    ma_object_set(
        string_object, "замінити",
        MA_MAKE_OBJECT(MaDiiaNative::Create(
            M, "замінити", ma_string_replace_diia_native_fn, string_object)));
    ma_object_set(string_object, "починається",
                  MA_MAKE_OBJECT(MaDiiaNative::Create(
                      M, "починається", ma_string_starts_with_diia_native_fn,
                      string_object)));
    ma_object_set(string_object, "закінчується",
                  MA_MAKE_OBJECT(MaDiiaNative::Create(
                      M, "закінчується", ma_string_ends_with_diia_native_fn,
                      string_object)));
    ma_object_set(
        string_object, "обтяти",
        MA_MAKE_OBJECT(MaDiiaNative::Create(
            M, "обтяти", ma_string_trim_diia_native_fn, string_object)));
    ma_object_set(
        string_object, MAG_ADD,
        MA_MAKE_OBJECT(MaDiiaNative::Create(
            M, MAG_ADD, ma_string_mag_add_diia_native_fn, string_object)));
    ma_object_set(string_object, MAG_CONTAINS,
                  MA_MAKE_OBJECT(MaDiiaNative::Create(
                      M, MAG_CONTAINS, ma_string_mag_contains_diia_native_fn,
                      string_object)));
    ma_object_set(
        string_object, MAG_GET_ELEMENT,
        MA_MAKE_OBJECT(MaDiiaNative::Create(
            M, MAG_GET_ELEMENT, ma_string_mag_get_element_diia_native_fn,
            string_object)));
    ma_object_set(string_object, MAG_ITERATOR,
                  MA_MAKE_OBJECT(MaDiiaNative::Create(
                      M, MAG_ITERATOR, ma_string_mag_iterator_diia_native_fn,
                      string_object)));
    ma_object_set(string_object, MAG_NUMBER,
                  MA_MAKE_OBJECT(MaDiiaNative::Create(
                      M, MAG_NUMBER, ma_string_mag_number_diia_native_fn,
                      string_object)));
    return string_object;
  }

  MaCell text_structure_object_mag_call_diia_native_fn(MaMa* M,
                                                       MaObject* o,
                                                       MaArgs* args) {
    const auto cell = MA_ARGS_GET(args, 0, "значення", MA_MAKE_EMPTY());
    if (IS_NUMBER(cell)) {
      RETURN(MA_MAKE_OBJECT(
          MaText::Create(M, ma_number_to_string(cell.v.number))));
    }
    if (IS_YES(cell)) {
      RETURN(MA_MAKE_OBJECT(MaText::Create(M, "так")));
    }
    if (IS_NO(cell)) {
      RETURN(MA_MAKE_OBJECT(MaText::Create(M, "ні")));
    }
    if (IS_OBJECT(cell)) {
      if (cell.v.object->type == MA_OBJECT_STRING) {
        RETURN(cell);
      } else if (OBJECT_HAS(cell.v.object, MAG_TEXT)) {
        return ma_call_handler(M, cell.v.object->properties[MAG_TEXT], {}, {});
      }
    }
    M->throw_cell =
        MA_MAKE_OBJECT(MaText::Create(M, "Неможливо перетворити на текст."));
    throw MaException();
  }

  void MaText::Init(MaMa* M) {
    const auto text_structure_object = MaStructure::Create(M, "текст");
    M->global_scope->set_variable("текст",
                                  MA_MAKE_OBJECT(text_structure_object));
    M->text_structure_object = text_structure_object;
    ma_object_set(
        text_structure_object, MAG_CALL,
        MA_MAKE_OBJECT(MaDiiaNative::Create(
            M, MAG_CALL, text_structure_object_mag_call_diia_native_fn,
            text_structure_object)));
  }
} // namespace mavka::mama