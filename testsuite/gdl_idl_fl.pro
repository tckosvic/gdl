;+
; Alain C., March 2015
;
; * AC 2017-JUL-27 adding /uppercase
; * AC 2017-JUL-27 adding /prefix
;
; Easy way to generate prefix/suffix for outputs files
; (e.g. running test suite)
;
; Most tests in the testsuite are running well
; in GDL, IDL and FL. But we have few differences ...
;
; ----------------------------------------------------
; Modifications history :
;
; 2017-JUL-27 : AC. adding /uppercase
; 2017-JUL-27 : AC. adding /prefix
; 2018-Feb-05 : AC. Default return now UpperCase
; 2018-Sep-06 : AC. new test for FL (mail from Lajos)
;               using undocument trick in FL
; 2018-Sep-06 : AC. adding /title 
; 2019-Nov-19 : AC. Since FL 0.79.46, we do have a !FL :)
; 2020-Mar-24 : AC. why not the list ?! (for cross test)
;
; ----------------------------------------------------
;-
function GDL_IDL_FL, uppercase=uppercase, lowercase=lowercase, $
                     prefix=prefix, list=list, title=title, $
                     verbose=verbose, test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'function GDL_IDL_FL, uppercase=uppercase, lowercase=lowercase, $'
   print, '                     prefix=prefix, list=list, title=title, $'
   print, '                     verbose=verbose, test=test, help=help'
   return, -1
endif
;
if KEYWORD_SET(list) then return, ['GDL', 'IDL', 'FL']
;
DEFSYSV, '!gdl', exists=isGDL
;
if isGDL then suffix='gdl' else begin
   ;;
   ;; AC 15 May 2015
   ;; (the only way we know to distinguish between IDL and FL)
   ;; it seems that, if not GDL, testing !Slave is simplest way to
   ;; check if we are in FL or IDL ! (better test welcome !)
   ;; This test is still OK in FL fl_0.79.41
   ;;
   ;; new way since FL 0.79.46
   DEFSYSV, '!FL', exists=isFL
   if isFL then begin
      suffix='fl'
   endif else begin
      ;; older ways ... may be unacurate :(
      
      DEFSYSV, '!slave', exists=isFL
      if isFL then begin
         suffix='fl'
      endif else begin
         ;;  new way to detect FL 
         ;; AC: FL trick : don't change next line !!!!
         in_fl=0              ;#fl +1
         ;; AC: FL trick : don't change previous line !!!!
         if in_fl then begin
            suffix='fl'
         endif else begin
            suffix='idl'
         endelse
      endelse
   endelse
endelse
;
; AC 2018-02-07 : we decided the default is now UpperCase
; We even don't test the /upperCase keyword (compatility)
suffix=STRUPCASE(suffix)
;
if KEYWORD_SET(prefix) then suffix=suffix+'_'
;
if KEYWORD_SET(title) then suffix=suffix+' : '
;
if KEYWORD_SET(lowercase) then suffix=STRLOWCASE(suffix)
;
if KEYWORD_SET(verbose) then MESSAGE, /cont, 'Detected Software : '+suffix
;
if KEYWORD_SET(test) then STOP
;
return, suffix
;
end
