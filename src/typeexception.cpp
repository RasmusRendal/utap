// -*- mode: C++; c-file-style: "stroustrup"; c-basic-offset: 4; indent-tabs-mode: nil; -*-

/* libutap - Uppaal Timed Automata Parser.
   Copyright (C) 2002-2006 Uppsala University and Aalborg University.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA
*/

#include "utap/builder.h"

namespace UTAP
{
    TypeException UnknownIdentifierError(const std::string& name)
    {
        return TypeException{"$Unknown_identifier: %1%" + name};
    }

    TypeException HasNoMemberError(const std::string& name) { return TypeException{"$has_no_member_named %1%" + name}; }

    TypeException IsNotAStructError(const std::string& name) { return TypeException{"%1% $is_not_a_structure" + name}; }

    TypeException DuplicateDefinitionError(const std::string& name)
    {
        return TypeException{"$Duplicate_definition_of %1%" + name};
    }

    TypeException InvalidTypeError(const std::string& name) { return TypeException{"$Invalid_type %1%" + name}; }

    TypeException NoSuchProcessError(const std::string& name) { return TypeException{"$No_such_process: %1%" + name}; }

    TypeException NotATemplateError(const std::string& name) { return TypeException{"$Not_a_template: %1%" + name}; }

    TypeException NotAProcessError(const std::string& name) { return TypeException{"%1% $is_not_a_process" + name}; }

    TypeException StrategyNotDeclaredError(const std::string& name)
    {
        return TypeException{"$strategy_not_declared: %1%" + name};
    }

    TypeException UnknownDynamicTemplateError(const std::string& name)
    {
        return TypeException{"Unknown dynamic template %1%" + name};
    }

    TypeException ShadowsAVariableWarning(const std::string& name)
    {
        return TypeException{"%1% $shadows_a_variable" + name};
    }

    TypeException CouldNotLoadLibraryError(const std::string& name)
    {
        return TypeException{"$Could_not_load_library_named %1%" + name};
    }

    TypeException CouldNotLoadFunctionError(const std::string& name)
    {
        return TypeException{"$Could_not_load_function_named %1%" + name};
    }

}  // namespace UTAP
