(define (set-color r g b)
  (bitwise-ior
   (bitwise-ior (bitwise-and (ash r 8) 63488)
		(bitwise-and (ash g 3) 2016))
   (bitwise-and b 31)))

(define (rgb x)
  (bitwise-ior
   (bitwise-ior (bitwise-and x 16711680) (bitwise-and x 65280))
   (bitwise-and x 255)))

(define (putpixel x y color)
  (#%putpixel x y color))
