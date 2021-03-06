------------------------------------------------------------------------------
--                                                                          --
--                 GNAT RUN-TIME LIBRARY (GNARL) COMPONENTS                 --
--                                                                          --
--                 S Y S T E M . T R A C E S . T A S K I N G                --
--                                                                          --
--                                  B o d y                                 --
--                                                                          --
--          Copyright (C) 2001-2005 Free Software Foundation, Inc.          --
--                                                                          --
-- GNARL is free software; you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 2,  or (at your option) any later ver- --
-- sion. GNARL is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License --
-- for  more details.  You should have  received  a copy of the GNU General --
-- Public License  distributed with GNARL; see file COPYING.  If not, write --
-- to  the  Free Software Foundation,  51  Franklin  Street,  Fifth  Floor, --
-- Boston, MA 02110-1301, USA.                                              --
--                                                                          --
--
--
--
--
--
--
--
-- GNAT was originally developed  by the GNAT team at  New York University. --
-- Extensive contributions were provided by Ada Core Technologies Inc.      --
--                                                                          --
------------------------------------------------------------------------------

package body System.Traces.Tasking is

   pragma Warnings (Off); -- kill warnings on unreferenced formals

   ---------------------
   -- Send_Trace_Info --
   ---------------------

   procedure Send_Trace_Info (Id : Trace_T; Task_Name2 : ST.Task_Id) is
   begin
      null;
   end Send_Trace_Info;

   procedure Send_Trace_Info
     (Id           : Trace_T;
      Task_Name2   : ST.Task_Id;
      Entry_Number : ST.Entry_Index)
   is
   begin
      null;
   end Send_Trace_Info;

   procedure Send_Trace_Info
     (Id           : Trace_T;
      Task_Name    : ST.Task_Id;
      Task_Name2   : ST.Task_Id;
      Entry_Number : ST.Entry_Index)
   is
   begin
      null;
   end Send_Trace_Info;

   procedure Send_Trace_Info
     (Id         : Trace_T;
      Task_Name  : ST.Task_Id;
      Task_Name2 : ST.Task_Id)
   is
   begin
      null;
   end Send_Trace_Info;

   procedure Send_Trace_Info
     (Id           : Trace_T;
      Entry_Number : ST.Entry_Index)
   is
   begin
      null;
   end Send_Trace_Info;

   procedure Send_Trace_Info
     (Id           : Trace_T;
      Acceptor     : ST.Task_Id;
      Entry_Number : ST.Entry_Index;
      Timeout      : Duration)
   is
   begin
      null;
   end Send_Trace_Info;

   procedure Send_Trace_Info
     (Id           : Trace_T;
      Entry_Number : ST.Entry_Index;
      Timeout      : Duration)
   is
   begin
      null;
   end Send_Trace_Info;

   procedure Send_Trace_Info
     (Id        : Trace_T;
      Task_Name : ST.Task_Id;
      Number    : Integer)
   is
   begin
      null;
   end Send_Trace_Info;

   procedure Send_Trace_Info
     (Id        : Trace_T;
      Task_Name : ST.Task_Id;
      Number    : Integer;
      Timeout   : Duration)
   is
   begin
      null;
   end Send_Trace_Info;

end System.Traces.Tasking;
