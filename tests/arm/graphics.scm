(define test-lcd-basic
  (lambda ()
    (let loop-x ((x 0) (count 0))
      (if (< x 240)
	  (begin
	    (let loop-y ((y 0) (mask 65535))
	      (if (< y 320)
		  (begin
		    (putpixel x y (bitwise-and count mask))
		    (loop-y (+ y 1) (- mask 1)))))
	    (loop-x (+ 1 x) (+ 1 count)))))))
