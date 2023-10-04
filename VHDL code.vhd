
---------------------------------------------------------
-- Prova Finale (Progetto di Reti Logiche) AA 2022-2023
-- Stefano Morano - Codice Persona 10737463
-- Alessandro Mancini - Codice Persona 10733387
-- Prof. Fabio Salice
-- Module Name: project_reti_logiche
-- Project Name: 10733387_10737463.vhd
---------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity project_reti_logiche is
    port (
        i_clk : in std_logic;
        i_rst : in std_logic;
        i_start : in std_logic;
        i_w : in std_logic;
        i_mem_data : in std_logic_vector (7 downto 0);
        o_z0 : out std_logic_vector (7 downto 0);
        o_z1 : out std_logic_vector (7 downto 0);
        o_z2 : out std_logic_vector (7 downto 0);
        o_z3 : out std_logic_vector (7 downto 0);
        o_done : out std_logic;
        o_mem_addr : out std_logic_vector (15 downto 0);
        o_mem_we : out std_logic;
        o_mem_en : out std_logic
    );
end project_reti_logiche;



architecture Behavioral of project_reti_logiche is
    signal reg_0 : std_logic_vector (7 downto 0);       	--registro che memorizza l'ultimo valore di o_zo
    signal reg_1 : std_logic_vector (7 downto 0);       	--registro che memorizza l'ultimo valore di o_z1
    signal reg_2 : std_logic_vector (7 downto 0);       	--registro che memorizzaa l'ultimo valore di o_z2
    signal reg_3 : std_logic_vector (7 downto 0);       	--registro che memorizza l'ultimo valore di o_z3
    signal reg_uscita_load : std_logic;                 	--segnale per attivare lo shift register reg_uscita
    signal reg_uscita_reset : std_logic;                	--segnale per resettare lo shift register reg_uscita
    signal reg_address_load : std_logic;                	--segnale per attivare lo shift register reg_address
    signal reg_address_reset : std_logic;               	--segnale per resettare lo shift register reg_address
    signal reg_0_load : std_logic;                      	--segnale per caricare i_mem_address in reg_0
    signal reg_1_load : std_logic;                      	--segnale per caricare i_mem_address in reg_1 
    signal reg_2_load : std_logic;                      	--segnale per caricare i_mem_address in reg_2
    signal reg_3_load : std_logic;                      	--segnale per caricare i_mem_address in reg_3 
    signal reg_address_copy : std_logic;                	--segnale per caricare reg_uscita in o_mem_addr
    signal reg_uscita_show : std_logic;                 	--registro per gestire l'output delle uscite
    signal reg_decoder_sel : std_logic;                 	--segnale per attivare il decoder   
    signal reg_uscita : std_logic_vector (1 downto 0);  	--registro per salvare il numero dell'uscita in binario 
    signal reg_address: std_logic_vector (15 downto 0); 	--registro in cui viene shiftato l'i_w per ottenere l'indirizzo della memoria
    type S is (S0, S1, S2, S3, S4, S5, S6, s7, S8);
    signal cur_state, next_state : S;
begin   

    process(i_clk, i_rst) begin                         	--gestione di clock e reset
        if (i_rst = '1') then
            cur_state <= S0;
        elsif (i_clk'event and i_clk = '1') then
            cur_state <= next_state;
        end if;
    end process;

    process (i_start, cur_state) begin                  	--gestione del cambio di stato
        next_state <= cur_state; 
        case cur_state is
            when S0 =>
                if (i_start = '1') then
                    next_state <= S1;
                end if;
            when S1 =>
                next_state <= S2;
            when S2 =>
                if (i_start = '0') then
                    next_state <= S4;
                elsif (i_start = '1') then
                    next_state <= S3;
                end if;
            when S3 =>
                if (i_start = '0') then
                    next_state <= S4;
                end if;
            when S4 =>                      
                    next_state <= S5;             
            when S5 =>
               next_state <= S6;
            when S6 =>
                    next_state <= S7;
            when S7 =>
                    next_state <= S8;
            when S8 =>
                    next_state <= S0;
            when others =>
        end case;
    end process;
   
    process (cur_state)                              	 --gestione dei segnali nei diversi stati
    begin
        o_mem_we <= '0';
        o_mem_en <= '0';
        reg_uscita_show <= '0';
        o_done <= '0';
        reg_uscita_load <= '0';
        reg_address_load <= '0';
        reg_uscita_reset <= '0';
        reg_address_reset <= '0';
        reg_address_copy <= '0';
        reg_decoder_sel <= '0'; 
        case cur_state is
            when S0 =>
                reg_uscita_load <= '1';
            when S1 =>  
                reg_uscita_load <= '1';
            when S2 =>
                reg_address_load <= '1';
            when S3 =>     
                reg_address_load <= '1';
            when S4 =>     
                 reg_address_copy <= '1';          
            when S5 =>
                o_mem_en <= '1';
            when s6 => 
                reg_decoder_sel <= '1'; 
            when s7 =>         
            when s8 =>
                reg_uscita_show <= '1';
                o_done <= '1';
                reg_uscita_reset <= '1';
                reg_address_reset <= '1';
             when others =>        
        end case; 
    end process;

    process (i_clk, i_rst)                          		--gestione di reg_0
    begin
        if (i_rst = '1') then
            reg_0 <= "00000000";
        elsif i_clk'event and i_clk = '1' then
            if (reg_0_load = '1') then
                reg_0 <= i_mem_data;
            end if;
        end if;
    end process;

    process (i_clk, i_rst)                           	--gestione di reg_1
    begin
        if (i_rst = '1') then
            reg_1 <= "00000000";
        elsif (i_clk'event and i_clk = '1') then
            if (reg_1_load = '1') then
                reg_1 <= i_mem_data;
            end if;
        end if;
    end process;
     
    process (i_clk, i_rst)                           	--gestione di reg_2
    begin
        if (i_rst = '1') then
            reg_2 <= "00000000";
        elsif (i_clk'event and i_clk = '1') then
            if (reg_2_load = '1') then
                reg_2 <= i_mem_data;
            end if;
        end if;
    end process;

    process (i_clk, i_rst)                           	--gestione di reg_3
    begin
        if(i_rst = '1') then
            reg_3 <= "00000000";
        elsif (i_clk'event and i_clk = '1') then
            if(reg_3_load = '1') then
                reg_3 <= i_mem_data;
            end if;
        end if;
    end process;    
      
    with reg_uscita_show select                            --gestione dei 4 mux
        o_z0 <= "00000000" when '0',
        reg_0(7 downto 0) when '1',
        "XXXXXXXX" when others;               
    with reg_uscita_show select                           
        o_z1 <= "00000000" when '0',
        reg_1(7 downto 0) when '1',
        "XXXXXXXX" when others;     
    with reg_uscita_show select                            
        o_z2 <= "00000000" when '0',
        reg_2(7 downto 0) when '1',
        "XXXXXXXX" when others;
    with reg_uscita_show select                            
        o_z3 <= "00000000" when '0',
        reg_3(7 downto 0) when '1',
        "XXXXXXXX" when others;            
                 
    process (i_rst, i_clk)                                 --gestione di reg_uscita
    begin
        if (i_rst = '1') then
            reg_uscita <= "00";
        elsif (i_clk'event and i_clk='1') then
            if (reg_uscita_load = '1' and i_start = '1') then
                reg_uscita <= reg_uscita(0) & i_w;
            elsif (reg_uscita_reset = '1') then
                reg_uscita <= "00";
            end if;
        end if;
    end process; 
    
    process (i_clk, i_rst)                                	 --gestione di reg_address
    begin
        if (i_rst = '1') then
            reg_address <= "0000000000000000";
        elsif (i_clk'event and i_clk = '1') then
            if (reg_address_load = '1' and i_start = '1') then
                reg_address <= reg_address(14 downto 0) & i_w;
            elsif (reg_address_reset = '1') then
                reg_address <= "0000000000000000";
            end if;
        end if;
    end process;
                  
    process (i_clk, i_rst)                                  --gestione di o_mem_address
    begin
        if (i_rst = '1') then
            o_mem_addr <= "0000000000000000";
        elsif (i_clk'event and i_clk = '1') then
            if (reg_address_copy = '1') then
                o_mem_addr <= reg_address;
            end if;
        end if;
    end process;
  
   process (reg_decoder_sel)                                --gestione del decoder
   begin
       reg_0_load <= '0';
       reg_1_load <= '0';
       reg_2_load <= '0';
       reg_3_load <= '0';
       if (reg_decoder_sel = '1') then
           case reg_uscita is
                when "00" =>
                   reg_0_load <= '1';
                when "01" =>
                   reg_1_load <= '1';
                when "10" =>
                   reg_2_load <= '1';
                when "11" =>
                   reg_3_load <= '1';
                when others =>
            end case;
       end if;
   end process; 
   
end Behavioral;