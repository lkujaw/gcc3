------------------------------------------------------------------------------
--                                                                          --
--                         GNAT RUN-TIME COMPONENTS                         --
--                                                                          --
--                         A D A . C A L E N D A R                          --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--          Copyright (C) 1992-2006, Free Software Foundation, Inc.         --
--                                                                          --
-- GNAT is free software;  you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 2,  or (at your option) any later ver- --
-- sion.  GNAT is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License --
-- for  more details.  You should have  received  a copy of the GNU General --
-- Public License  distributed with GNAT;  see file COPYING.  If not, write --
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

with Unchecked_Conversion;

with System.OS_Primitives;
--  used for Clock

package body Ada.Calendar is

   ------------------------------
   -- Use of Pragma Unsuppress --
   ------------------------------

   --  This implementation of Calendar takes advantage of the permission in
   --  Ada 95 of using arithmetic overflow checks to check for out of bounds
   --  time values. This means that we must catch the constraint error that
   --  results from arithmetic overflow, so we use pragma Unsuppress to make
   --  sure that overflow is enabled, using software overflow checking if
   --  necessary. That way, compiling Calendar with options to suppress this
   --  checking will not affect its correctness.

   ------------------------
   -- Local Declarations --
   ------------------------

   type Char_Pointer is access Character;
   subtype int  is Integer;
   subtype long is Long_Integer;
   --  Synonyms for C types. We don't want to get them from Interfaces.C
   --  because there is no point in loading that unit just for calendar.

   type tm is record
      tm_sec    : int;           -- seconds after the minute (0 .. 60)
      tm_min    : int;           -- minutes after the hour (0 .. 59)
      tm_hour   : int;           -- hours since midnight (0 .. 24)
      tm_mday   : int;           -- day of the month (1 .. 31)
      tm_mon    : int;           -- months since January (0 .. 11)
      tm_year   : int;           -- years since 1900
      tm_wday   : int;           -- days since Sunday (0 .. 6)
      tm_yday   : int;           -- days since January 1 (0 .. 365)
      tm_isdst  : int;           -- Daylight Savings Time flag (-1 .. +1)
      tm_gmtoff : long;          -- offset from CUT in seconds
      tm_zone   : Char_Pointer;  -- timezone abbreviation
   end record;

   type tm_Pointer is access all tm;

   subtype time_t is long;

   type time_t_Pointer is access all time_t;

   procedure localtime_r (C : time_t_Pointer; res : tm_Pointer);
   pragma Import (C, localtime_r, "__gnat_localtime_r");

   function mktime (TM : tm_Pointer) return time_t;
   pragma Import (C, mktime);
   --  mktime returns -1 in case the calendar time given by components of
   --  TM.all cannot be represented.

   --  The following constants are used in adjusting Ada dates so that they
   --  fit into a 56 year range that can be handled by Unix (1970 included -
   --  2026 excluded). Dates that are not in this 56 year range are shifted
   --  by multiples of 56 years to fit in this range.

   --  The trick is that the number of days in any four year period in the Ada
   --  range of years (1901 - 2099) has a constant number of days. This is
   --  because we have the special case of 2000 which, contrary to the normal
   --  exception for centuries, is a leap year after all. 56 has been chosen,
   --  because it is not only a multiple of 4, but also a multiple of 7. Thus
   --  two dates 56 years apart fall on the same day of the week, and the
   --  Daylight Saving Time change dates are usually the same for these two
   --  years.

   Unix_Year_Min : constant := 1970;
   Unix_Year_Max : constant := 2026;

   Ada_Year_Min : constant := 1901;
   Ada_Year_Max : constant := 2099;

   --  Some basic constants used throughout

   Days_In_Month : constant array (Month_Number) of Day_Number :=
                     (31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);

   Days_In_4_Years      : constant := 365 * 3 + 366;
   Seconds_In_4_Years   : constant := 86_400 * Days_In_4_Years;
   Seconds_In_56_Years  : constant := Seconds_In_4_Years * 14;
   Seconds_In_56_YearsD : constant := Duration (Seconds_In_56_Years);

   ---------
   -- "+" --
   ---------

   function "+" (Left : Time; Right : Duration) return Time is
      pragma Unsuppress (Overflow_Check);
   begin
      return (Left + Time (Right));
   exception
      when Constraint_Error =>
         raise Time_Error;
   end "+";

   function "+" (Left : Duration; Right : Time) return Time is
      pragma Unsuppress (Overflow_Check);
   begin
      return (Time (Left) + Right);
   exception
      when Constraint_Error =>
         raise Time_Error;
   end "+";

   ---------
   -- "-" --
   ---------

   function "-" (Left : Time; Right : Duration)  return Time is
      pragma Unsuppress (Overflow_Check);
   begin
      return Left - Time (Right);
   exception
      when Constraint_Error =>
         raise Time_Error;
   end "-";

   function "-" (Left : Time; Right : Time) return Duration is
      pragma Unsuppress (Overflow_Check);
   begin
      return Duration (Left) - Duration (Right);
   exception
      when Constraint_Error =>
         raise Time_Error;
   end "-";

   ---------
   -- "<" --
   ---------

   function "<" (Left, Right : Time) return Boolean is
   begin
      return Duration (Left) < Duration (Right);
   end "<";

   ----------
   -- "<=" --
   ----------

   function "<=" (Left, Right : Time) return Boolean is
   begin
      return Duration (Left) <= Duration (Right);
   end "<=";

   ---------
   -- ">" --
   ---------

   function ">" (Left, Right : Time) return Boolean is
   begin
      return Duration (Left) > Duration (Right);
   end ">";

   ----------
   -- ">=" --
   ----------

   function ">=" (Left, Right : Time) return Boolean is
   begin
      return Duration (Left) >= Duration (Right);
   end ">=";

   -----------
   -- Clock --
   -----------

   function Clock return Time is
   begin
      return Time (System.OS_Primitives.Clock);
   end Clock;

   ---------
   -- Day --
   ---------

   function Day (Date : Time) return Day_Number is
      DY : Year_Number;
      DM : Month_Number;
      DD : Day_Number;
      DS : Day_Duration;
   begin
      Split (Date, DY, DM, DD, DS);
      return DD;
   end Day;

   -----------
   -- Month --
   -----------

   function Month (Date : Time) return Month_Number is
      DY : Year_Number;
      DM : Month_Number;
      DD : Day_Number;
      DS : Day_Duration;
   begin
      Split (Date, DY, DM, DD, DS);
      return DM;
   end Month;

   -------------
   -- Seconds --
   -------------

   function Seconds (Date : Time) return Day_Duration is
      DY : Year_Number;
      DM : Month_Number;
      DD : Day_Number;
      DS : Day_Duration;
   begin
      Split (Date, DY, DM, DD, DS);
      return DS;
   end Seconds;

   -----------
   -- Split --
   -----------

   procedure Split
     (Date    : Time;
      Year    : out Year_Number;
      Month   : out Month_Number;
      Day     : out Day_Number;
      Seconds : out Day_Duration)
   is
      Offset : Long_Integer;

   begin
      Split_W_Offset (Date, Year, Month, Day, Seconds, Offset);
   end Split;

   --------------------
   -- Split_W_Offset --
   --------------------

   procedure Split_W_Offset
     (Date    : Time;
      Year    : out Year_Number;
      Month   : out Month_Number;
      Day     : out Day_Number;
      Seconds : out Day_Duration;
      Offset  : out Long_Integer)
   is
      --  The following declare bounds for duration that are comfortably
      --  wider than the maximum allowed output result for the Ada range
      --  of representable split values. These are used for a quick check
      --  that the value is not wildly out of range.

      Low  : constant := (Ada_Year_Min - Unix_Year_Min - 2) * 365 * 86_400;
      High : constant := (Ada_Year_Max - Unix_Year_Min + 2) * 365 * 86_400;

      LowD  : constant Duration := Duration (Low);
      HighD : constant Duration := Duration (High);

      --  Finally the actual variables used in the computation

      Adjusted_Seconds : aliased time_t;
      D                : Duration;
      Frac_Sec         : Duration;
      Year_Val         : Integer;
      Tm_Val           : aliased tm;

   begin
      --  For us a time is simply a signed duration value, so we work with
      --  this duration value directly. Note that it can be negative.

      D := Duration (Date);

      --  First of all, filter out completely ludicrous values. Remember that
      --  we use the full stored range of duration values, which may be
      --  significantly larger than the allowed range of Ada times. Note that
      --  these checks are wider than required to make absolutely sure that
      --  there are no end effects from time zone differences.

      if D < LowD or else D > HighD then
         raise Time_Error;
      end if;

      --  The unix localtime_r function is more or less exactly what we need
      --  here. The less comes from the fact that it does not support the
      --  required range of years (the guaranteed range available is only
      --  EPOCH through EPOCH + N seconds). N is in practice 2 ** 31 - 1.

      --  If we have a value outside this range, then we first adjust it to be
      --  in the required range by adding multiples of 56 years. For the range
      --  we are interested in, the number of days in any consecutive 56 year
      --  period is constant. Then we do the split on the adjusted value, and
      --  readjust the years value accordingly.

      Year_Val := 0;

      while D < 0.0 loop
         D := D + Seconds_In_56_YearsD;
         Year_Val := Year_Val - 56;
      end loop;

      while D >= Seconds_In_56_YearsD loop
         D := D - Seconds_In_56_YearsD;
         Year_Val := Year_Val + 56;
      end loop;

      --  Now we need to take the value D, which is now non-negative, and
      --  break it down into seconds (to pass to the localtime_r function) and
      --  fractions of seconds (for the adjustment below).

      --  Surprisingly there is no easy way to do this in Ada, and certainly
      --  no easy way to do it and generate efficient code. Therefore we do it
      --  at a low level, knowing that it is really represented as an integer
      --  with units of Small

      declare
         type D_Int is range 0 .. 2 ** (Duration'Size - 1) - 1;
         for D_Int'Size use Duration'Size;

         Small_Div : constant D_Int := D_Int (1.0 / Duration'Small);
         D_As_Int  : D_Int;

         function To_D_As_Int is new Unchecked_Conversion (Duration, D_Int);
         function To_Duration is new Unchecked_Conversion (D_Int, Duration);

      begin
         D_As_Int := To_D_As_Int (D);
         Adjusted_Seconds := time_t (D_As_Int / Small_Div);
         Frac_Sec := To_Duration (D_As_Int rem Small_Div);
      end;

      localtime_r (Adjusted_Seconds'Unchecked_Access, Tm_Val'Unchecked_Access);

      Year_Val := Tm_Val.tm_year + 1900 + Year_Val;
      Month    := Tm_Val.tm_mon + 1;
      Day      := Tm_Val.tm_mday;
      Offset   := Tm_Val.tm_gmtoff;

      --  The Seconds value is a little complex. The localtime function
      --  returns the integral number of seconds, which is what we want, but
      --  we want to retain the fractional part from the original Time value,
      --  since this is typically stored more accurately.

      Seconds := Duration (Tm_Val.tm_hour * 3600 +
                           Tm_Val.tm_min  * 60 +
                           Tm_Val.tm_sec)
                   + Frac_Sec;

      --  Note: the above expression is pretty horrible, one of these days we
      --  should stop using time_of and do everything ourselves to avoid these
      --  unnecessary divides and multiplies???.

      --  The Year may still be out of range, since our entry test was
      --  deliberately crude. Trying to make this entry test accurate is
      --  tricky due to time zone adjustment issues affecting the exact
      --  boundary. It is interesting to note that whether or not a given
      --  Calendar.Time value gets Time_Error when split depends on the
      --  current time zone setting.

      if Year_Val not in Ada_Year_Min .. Ada_Year_Max then
         raise Time_Error;
      else
         Year := Year_Val;
      end if;
   end Split_W_Offset;

   -------------
   -- Time_Of --
   -------------

   function Time_Of
     (Year    : Year_Number;
      Month   : Month_Number;
      Day     : Day_Number;
      Seconds : Day_Duration := 0.0)
      return    Time
   is
      Result_Secs : aliased time_t;
      TM_Val      : aliased tm;
      Int_Secs    : constant Integer := Integer (Seconds);

      Year_Val        : Integer := Year;
      Duration_Adjust : Duration := 0.0;

   begin
      --  The following checks are redundant with respect to the constraint
      --  error checks that should normally be made on parameters, but we
      --  decide to raise Constraint_Error in any case if bad values come in
      --  (as a result of checks being off in the caller, or for other
      --  erroneous or bounded error cases).

      if        not Year   'Valid
        or else not Month  'Valid
        or else not Day    'Valid
        or else not Seconds'Valid
      then
         raise Constraint_Error;
      end if;

      --  Check for Day value too large (one might expect mktime to do this
      --  check, as well as the basic checks we did with 'Valid, but it seems
      --  that at least on some systems, this built-in check is too weak).

      if Day > Days_In_Month (Month)
        and then (Day /= 29 or Month /= 2 or Year mod 4 /= 0)
      then
         raise Time_Error;
      end if;

      TM_Val.tm_sec  := Int_Secs mod 60;
      TM_Val.tm_min  := (Int_Secs / 60) mod 60;
      TM_Val.tm_hour := (Int_Secs / 60) / 60;
      TM_Val.tm_mday := Day;
      TM_Val.tm_mon  := Month - 1;

      --  For the year, we have to adjust it to a year that Unix can handle.
      --  We do this in 56 year steps, since the number of days in 56 years is
      --  constant, so the timezone effect on the conversion from local time
      --  to GMT is unaffected; also the DST change dates are usually not
      --  modified.

      while Year_Val < Unix_Year_Min loop
         Year_Val := Year_Val + 56;
         Duration_Adjust := Duration_Adjust - Seconds_In_56_YearsD;
      end loop;

      while Year_Val >= Unix_Year_Max loop
         Year_Val := Year_Val - 56;
         Duration_Adjust := Duration_Adjust + Seconds_In_56_YearsD;
      end loop;

      TM_Val.tm_year := Year_Val - 1900;

      --  If time is very close to UNIX epoch mktime may behave uncorrectly
      --  because of the way the different time zones are handled (a date
      --  after epoch in a given time zone may correspond to a GMT date
      --  before epoch). Adding one day to the date (this amount is latter
      --  substracted) avoids this problem.

      if Year_Val = Unix_Year_Min
        and then Month = 1
        and then Day = 1
      then
         TM_Val.tm_mday := TM_Val.tm_mday + 1;
         Duration_Adjust := Duration_Adjust - Duration (86400.0);
      end if;

      --  Since we do not have information on daylight savings, rely on the
      --  default information.

      TM_Val.tm_isdst := -1;
      Result_Secs := mktime (TM_Val'Unchecked_Access);

      --  That gives us the basic value in seconds. Two adjustments are
      --  needed. First we must undo the year adjustment carried out above.
      --  Second we put back the fraction seconds value since in general the
      --  Day_Duration value we received has additional precision which we do
      --  not want to lose in the constructed result.

      return
        Time (Duration (Result_Secs) +
              Duration_Adjust +
              (Seconds - Duration (Int_Secs)));
   end Time_Of;

   ----------
   -- Year --
   ----------

   function Year (Date : Time) return Year_Number is
      DY : Year_Number;
      DM : Month_Number;
      DD : Day_Number;
      DS : Day_Duration;
   begin
      Split (Date, DY, DM, DD, DS);
      return DY;
   end Year;

   -------------------
   --  Leap_Sec_Ops --
   -------------------

   --  The package that is used by the Ada 2005 children of Ada.Calendar:
   --  Ada.Calendar.Arithmetic and Ada.Calendar.Formatting.

   package body Leap_Sec_Ops is

      --  This package will need updating as leap seconds are added. Adding
      --  a leap second requires incrementing the value of N_Leap_Secs and
      --  adding the day of the new leap second to the end of Leap_Second_
      --  Dates.

      --  Elaboration of this package takes care of converting the Leap_
      --  Second_Dates table to a form that is better suited for the
      --  procedures provided by this package (a table that would be more
      --  difficult to maintain by hand).

      N_Leap_Secs : constant := 23;

      type Leap_Second_Date is record
         Year  : Year_Number;
         Month : Month_Number;
         Day   : Day_Number;
      end record;

      Leap_Second_Dates :
        constant array (1 .. N_Leap_Secs) of Leap_Second_Date :=
          ((1972,  6, 30), (1972, 12, 31), (1973, 12, 31), (1974, 12, 31),
           (1975, 12, 31), (1976, 12, 31), (1977, 12, 31), (1978, 12, 31),
           (1979, 12, 31), (1981,  6, 30), (1982,  6, 30), (1983,  6, 30),
           (1985,  6, 30), (1987, 12, 31), (1989, 12, 31), (1990, 12, 31),
           (1992,  6, 30), (1993,  6, 30), (1994,  6, 30), (1995, 12, 31),
           (1997,  6, 30), (1998, 12, 31), (2005, 12, 31));

      Leap_Second_Times : array (1 .. N_Leap_Secs) of Time;
      --  This is the needed internal representation that is calculated
      --  from Leap_Second_Dates during elaboration;

      --------------------------
      -- Cumulative_Leap_Secs --
      --------------------------

      procedure Cumulative_Leap_Secs
        (Start_Date    : Time;
         End_Date      : Time;
         Leaps_Between : out Duration;
         Next_Leap_Sec : out Time)
      is
         End_T      : Time;
         K          : Positive;
         Leap_Index : Positive;
         Start_Tmp  : Time;
         Start_T    : Time;

         type D_Int is range 0 .. 2 ** (Duration'Size - 1) - 1;
         for  D_Int'Size use Duration'Size;

         Small_Div : constant D_Int := D_Int (1.0 / Duration'Small);
         D_As_Int  : D_Int;

         function To_D_As_Int is new Unchecked_Conversion (Duration, D_Int);

      begin
         Next_Leap_Sec := After_Last_Leap;

         --  We want to throw away the fractional part of seconds. Before
         --  proceding with this operation, make sure our working values
         --  are non-negative.

         if End_Date < 0.0 then
            Leaps_Between := 0.0;
            return;
         end if;

         if Start_Date < 0.0 then
            Start_Tmp := Time (0.0);
         else
            Start_Tmp := Start_Date;
         end if;

         if Start_Date <= Leap_Second_Times (N_Leap_Secs) then

            --  Manipulate the fixed point value as an integer, similar to
            --  Ada.Calendar.Split in order to remove the fractional part
            --  from the time we will work with, Start_T and End_T.

            D_As_Int := To_D_As_Int (Duration (Start_Tmp));
            D_As_Int := D_As_Int / Small_Div;
            Start_T  := Time (D_As_Int);
            D_As_Int := To_D_As_Int (Duration (End_Date));
            D_As_Int := D_As_Int / Small_Div;
            End_T    := Time (D_As_Int);

            Leap_Index := 1;
            loop
               exit when Leap_Second_Times (Leap_Index) >= Start_T;
               Leap_Index := Leap_Index + 1;
            end loop;

            K := Leap_Index;
            loop
               exit when K > N_Leap_Secs or else
                 Leap_Second_Times (K) >= End_T;
               K := K + 1;
            end loop;

            if K <= N_Leap_Secs then
               Next_Leap_Sec := Leap_Second_Times (K);
            end if;

            Leaps_Between := Duration (K - Leap_Index);
         else
            Leaps_Between := Duration (0.0);
         end if;
      end Cumulative_Leap_Secs;

      ----------------------
      -- All_Leap_Seconds --
      ----------------------

      function All_Leap_Seconds return Duration is
      begin
         return Duration (N_Leap_Secs);
         --  Presumes each leap second is +1.0 second;
      end All_Leap_Seconds;

   --  Start of processing in package Leap_Sec_Ops

   begin
      declare
         Days         : Natural;
         Is_Leap_Year : Boolean;
         Years        : Natural;

         Cumulative_Days_Before_Month :
           constant array (Month_Number) of Natural :=
             (0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334);
      begin
         for J in 1 .. N_Leap_Secs loop
            Years := Leap_Second_Dates (J).Year - Unix_Year_Min;
            Days  := (Years / 4) * Days_In_4_Years;
            Years := Years mod 4;
            Is_Leap_Year := False;

            if Years = 1 then
               Days := Days + 365;

            elsif Years = 2 then
               Is_Leap_Year := True;
               --  1972 or multiple of 4 after
               Days := Days + 365 * 2;

            elsif Years = 3 then
               Days := Days + 365 * 3 + 1;
            end if;

            Days := Days + Cumulative_Days_Before_Month
                             (Leap_Second_Dates (J).Month);

            if Is_Leap_Year
              and then Leap_Second_Dates (J).Month > 2
            then
               Days := Days + 1;
            end if;

            Days := Days + Leap_Second_Dates (J).Day;

            Leap_Second_Times (J) :=
              Time (Days * Duration (86_400.0) + Duration (J - 1));

            --  Add one to get to the leap second. Add J - 1 previous
            --  leap seconds.
         end loop;
      end;
   end Leap_Sec_Ops;

begin
   System.OS_Primitives.Initialize;
end Ada.Calendar;
