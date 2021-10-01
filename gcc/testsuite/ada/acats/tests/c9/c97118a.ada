-- C97118A.ADA

--                             Grant of Unlimited Rights
--
--     Under contracts F33600-87-D-0337, F33600-84-D-0280, MDA903-79-C-0687,
--     F08630-91-C-0015, and DCA100-97-D-0025, the U.S. Government obtained 
--     unlimited rights in the software and documentation contained herein.
--     Unlimited rights are defined in DFAR 252.227-7013(a)(19).  By making 
--     this public release, the Government intends to confer upon all 
--     recipients unlimited rights  equal to those held by the Government.  
--     These rights include rights to use, duplicate, release or disclose the 
--     released technical data and computer software in whole or in part, in 
--     any manner and for any purpose whatsoever, and to have or permit others 
--     to do so.
--
--                                    DISCLAIMER
--
--     ALL MATERIALS OR INFORMATION HEREIN RELEASED, MADE AVAILABLE OR
--     DISCLOSED ARE AS IS.  THE GOVERNMENT MAKES NO EXPRESS OR IMPLIED 
--     WARRANTY AS TO ANY MATTER WHATSOEVER, INCLUDING THE CONDITIONS OF THE
--     SOFTWARE, DOCUMENTATION OR OTHER INFORMATION RELEASED, MADE AVAILABLE 
--     OR DISCLOSED, OR THE OWNERSHIP, MERCHANTABILITY, OR FITNESS FOR A
--     PARTICULAR PURPOSE OF SAID MATERIAL.
--*
-- CHECK THAT A CALL TO A CLOSED ALTERNATIVE OF A SELECTIVE WAIT IS NOT
-- ACCEPTED.

-- WRG 7/11/86

WITH REPORT; USE REPORT;
PROCEDURE C97118A IS

BEGIN

     TEST ("C97118A", "CHECK THAT A CALL TO A CLOSED ALTERNATIVE OF " &
                      "A SELECTIVE WAIT IS NOT ACCEPTED");

     DECLARE

          TASK T IS
               ENTRY E;
          END T;

          TASK BODY T IS
          BEGIN
               -- ENSURE THAT E HAS BEEN CALLED BEFORE PROCEEDING:
               WHILE E'COUNT = 0 LOOP
                    DELAY 1.0;
               END LOOP;

               SELECT
                    WHEN IDENT_BOOL (FALSE) =>
                         ACCEPT E;
                         FAILED ("ACCEPTED CALL TO CLOSED ALTERNATIVE");
                    ELSE
                         NULL;
               END SELECT;

               IF E'COUNT = 1 THEN
                    ACCEPT E;
               END IF;
          END T;

     BEGIN

          T.E;

     END;

     RESULT;

END C97118A;
