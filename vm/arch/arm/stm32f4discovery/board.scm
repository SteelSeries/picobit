(define set-led!
  (lambda (led val)
    (#%set-led! led val)))

(define getchar
  (lambda ()
    (#%getchar 0)))

(define getchar-wait
  (lambda (delay)
    (#%getchar delay)))

(define readln
  (lambda (ch buf)
    (if (not ch)
	(readln (getchar) buf)
	(if (not (= ch 13))
	    (readln (getchar) (cons ch buf))
	    (list->string (reverse buf))))))

(define putchar
  (lambda (c)
    (#%putchar c)))

(define clock
  (lambda ()
    (#%clock)))

(define systick
  (lambda ()
    (#%systick)))

(define display
  (lambda (x)
    (if (string? x)
	(for-each putchar (string->list x))
	(write x))))

(define #%write-list
  (lambda (lst)
    (cond ((null? lst)
	   (#%putchar #\)))
	  ((pair? lst)
	   (begin (#%putchar #\space)
		  (write (car lst))
		  (#%write-list (cdr lst))))
	  (else
	   (begin (display " . ")
		  (write lst)
		  (#%putchar #\)))))))

(define write
  (lambda (x)
    (cond ((string? x)
	   (begin (#%putchar #\")
		  (display x)
		  (#%putchar #\")))
	  ((number? x)
	   (display (number->string x)))
	  ((pair? x)
	   (begin (#%putchar #\()
                  (write (car x))
                  (#%write-list (cdr x))))
	  ((symbol? x)
	   (display "#<symbol>"))
	  ((boolean? x)
	   (display (if x "#t" "#f")))
	  (else
	   (display "#<object>")))))

(define (newline)
  (#%putchar #\return)
  (#%putchar #\newline))

(define (displayln x) (display x) (newline))

(define pp
  (lambda (x)
    (write x)
    (newline)))

(define time->seconds (lambda (t) (quotient t 1000)))
