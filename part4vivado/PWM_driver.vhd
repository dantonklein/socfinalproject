library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity PWM_driver is
    generic (
        WIDTH : positive := 8 -- Number of bits in the number that must be counted through for 1 PWM cycle
    );
    port ( 
        clk        : in  std_logic;
        rst        : in  std_logic;
        duty_cycle : in  std_logic_vector(WIDTH-1 downto 0); -- Value that must be counted to before PWM outputs zero
        pwm_out    : out std_logic
    );
end PWM_driver;

architecture Behavioral of PWM_driver is
    constant clk_cnt_len : positive := 1; -- Controls number of clk cycles for one PWM count
    signal pwm_cnt : unsigned(WIDTH - 1 downto 0);

    -- Only used if clk_cnt_len > 1
    signal clk_cnt : integer range 0 to clk_cnt_len - 1;
begin

    -- Counts the number of
    count_process : process(clk)
    begin
        if (rising_edge(clk)) then
            if (rst = '1') then
                clk_cnt <= 0;
            else
                if (clk_cnt < clk_cnt_len - 1) then
                    clk_cnt <= clk_cnt + 1;
                else
                    clk_cnt <= 0;
                end if;
            end if;
        end if;
    end process;

    PWM_process : process(clk)
    begin
        if (rising_edge(clk)) then
            if (rst = '1') then
                pwm_cnt <= (others => '0');
                pwm_out <= '0';
            else
                if (clk_cnt = 0 or clk_cnt_len = 1) then -- clk_cnt_len = 1 reduces 
                    pwm_cnt <= pwm_cnt + 1;
                    pwm_out <= '0';
                    -- accounts for reg delay
                    if (pwm_cnt = unsigned(to_signed(-2, pwm_cnt'length))) then 
                        pwm_cnt <= (others => '0');
                    end if;
                    if (pwm_cnt < unsigned(duty_cycle)) then
                        pwm_out <= '1';
                    end if;
                end if;
            end if;
        end if;
    end process;

end Behavioral;
